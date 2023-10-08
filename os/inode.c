/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <mosnix/inode.h>
#include <mosnix/config.h>
#include <mosnix/attributes.h>
#include <mosnix/proc.h>
#include <mosnix/util.h>
#include <mosnix/printk.h>
#include <sys/stat.h>
#include <bits/fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

/** Temporary pathname buffer, to keep it off the stack */
char temp_path[CONFIG_PATH_MAX] ATTR_SECTION_NOINIT;

#if CONFIG_SYMLINK

/** Temporary symbolic link read buffer, to keep it off the stack */
static char symlink_buffer[CONFIG_PATH_MAX] ATTR_SECTION_NOINIT;

#endif

ATTR_NOINLINE struct inode *inode_alloc
    (const struct inode_operations *operations)
{
    struct inode *inode = kmalloc_buf_alloc();
    if (inode) {
        inode->count = 1;
        inode->op = operations;
    }
    return inode;
}

ATTR_NOINLINE int inode_deref(struct inode *inode)
{
    --(inode->count);
    if (inode->count <= 0) {
        int result = inode->op->release(inode);
        kmalloc_buf_free(inode);
        return result;
    } else {
        return 0;
    }
}

static ATTR_NOINLINE int inode_resolve_relative
    (char *out, size_t outlen, size_t posn, const char *pathname)
{
    int isdir = (posn == 1);
    size_t len;

    /* Resolve all components that are separated by slashes */
    for (;;) {
        /* Skip one or more '/' separators, which allows paths like
         * "abc//def///g/" to work. */
        while (pathname[0] == '/') {
            isdir = 1;
            ++pathname;
        }

        /* Have we reached the end of the pathname yet? */
        if (pathname[0] == '\0') {
            break;
        }

        /* Find the next slash or end of string */
        len = strlen_path_component(pathname, CONFIG_PATH_MAX);

        /* Check for "." and ".." */
        if (pathname[0] == '.') {
            if (len == 1) {
                /* "." does nothing to the output path */
                ++pathname;
                continue;
            } else if (len == 2 && pathname[1] == '.') {
                /* If we are at the root, then stay there.  Any number of
                 * ".." components will not go up any further than the root. */
                if (posn > 1) {
                    /* Strip the last component from the output path */
                    while (posn > 1 && out[posn - 1] != '/') {
                        --posn;
                    }
                    if (posn > 1) {
                        --posn;
                        isdir = 0;
                    } else {
                        isdir = 1;
                    }
                } else {
                    isdir = 1;
                }
                pathname += 2;
                continue;
            }
        }

        /* Add the incoming component to the in-progress output path */
        if (posn > 1)
            out[posn++] = '/';
        if ((posn + len) >= outlen)
            return -ENAMETOOLONG;
        memcpy(out + posn, pathname, len);
        posn += len;
        isdir = 0;

        /* Skip to the next pathname component */
        pathname += len;
    }
    out[posn] = '\0';
    return isdir;
}

ATTR_NOINLINE int inode_path_to_abs
    (char *out, size_t outlen, const char *pathname)
{
    size_t posn;

    /* Perform basic pathname validation */
    if (!pathname)
        return -EFAULT;
    if (pathname[0] == '\0')
        return -EINVAL;
    if (str_is_too_long(pathname, CONFIG_PATH_MAX - 1))
        return -ENAMETOOLONG;

    /* Absolute or relative?  Find the starting directory */
    if (pathname[0] == '/') {
        if (outlen < 2)
            return -ENAMETOOLONG;
        out[0] = '/';
        posn = 1;
        ++pathname;
    } else {
        posn = strlen_constrained(current_proc->cwd, sizeof(current_proc->cwd));
        if ((posn + 1) >= outlen)
            return -ENAMETOOLONG;
        memcpy(out, current_proc->cwd, posn);
    }

    /* Resolve the path relative to the starting point */
    return inode_resolve_relative(out, outlen, posn, pathname);
}

#if CONFIG_SYMLINK

ATTR_NOINLINE int inode_symlink_to_abs
    (char *out, size_t outlen, const char *pathname)
{
    size_t posn;

    /* If the symbolic link starts with '/', then resolve as absolute */
    if (!pathname)
        return -EFAULT;
    if (pathname[0] == '/')
        return inode_path_to_abs(out, outlen, pathname);

    /* Perform basic pathname validation */
    if (pathname[0] == '\0')
        return -EINVAL;
    if (str_is_too_long(pathname, CONFIG_PATH_MAX - 1))
        return -ENAMETOOLONG;

    /* Drop the last component of the base path as the symlink's base
     * is the parent directory. */
    posn = strlen_constrained(out, outlen);
    if (posn > 1) {
        while (posn > 1 && out[posn - 1] != '/') {
            --posn;
        }
        if (posn > 1) {
            --posn;
        }
    }

    /* Resolve the relative part of the symlink */
    return inode_resolve_relative(out, outlen, posn, pathname);
}

#endif /* CONFIG_SYMLINK */

int inode_lookup(struct inode **inode, char *pathname, int oflag,
                 mode_t mode, u_char follow)
{
    struct inode *node = inode_get_root();
    struct inode *child;
    size_t posn, namelen;
    const char *name;
    u_char is_last;
    int error;
#if CONFIG_SYMLINK
    unsigned symlink_depth = 0;
#endif

    /* Clear the inode return value */
    if (inode)
        *inode = NULL;

    /* Walk the directory tree */
    posn = 1; /* Skip the leading '/' */
    while (pathname[posn] != '\0') {
        /* If the current node is not a directory, then cannot proceed */
        if (!S_ISDIR(node->mode)) {
            inode_deref(node);
            return -ENOTDIR;
        }

        /* Does the user have search permissions on the current directory? */
        error = inode_access(node, X_OK);
        if (error < 0) {
            inode_deref(node);
            return error;
        }

        /* Extract the next pathname component */
        name = pathname + posn;
        namelen = strlen_path_component(name, CONFIG_PATH_MAX);
        is_last = (name[namelen] == '\0');

        /* Look up the child using the filesystem layer */
        error = node->op->lookup(&child, node, name, namelen);
        if (error == -ENOENT) {
            /* The node does not exist.  If this is the last component
             * in the path, then we might be able to create it. */
            if (is_last && (oflag & O_CREAT) != 0) {
                /* Can we write to the current directory? */
                error = inode_access(node, W_OK);
                if (error < 0) {
                    inode_deref(node);
                    return error;
                }
                if (!(node->op->mknod)) {
                    /* Filesystem is read-only */
                    inode_deref(node);
                    return -EROFS;
                }

                /* Create the new inode with the correct options. */
                error = node->op->mknod(&child, node, name, namelen, mode);
                inode_deref(node);
                if (error < 0) {
                    return error;
                }
                if (inode) {
                    *inode = child;
                } else {
                    inode_deref(child);
                }
                return 1; /* Indicate that a new node was created */
            }
        }
        if (error < 0) {
            inode_deref(node);
            return error;
        }

        /* Replace the current inode with the already-referenced child */
        inode_deref(node);
        node = child;

        /* Is the child a symbolic link? */
        if (S_ISLNK(node->mode)) {
#if CONFIG_SYMLINK
            if (is_last && !follow) {
                /* Stop here with the symbolic link, not the linked-to node */
                break;
            }
            if ((++symlink_depth) > CONFIG_MAX_SYMLINK_DEPTH) {
                inode_deref(node);
                return -ELOOP;
            }
            if (!(node->op->readlink)) {
                /* Symbolic links are not supported on this filesystem type */
                inode_deref(node);
                return -ENOENT;
            }

            /* Read the contents of the symbolic link */
            error = node->op->readlink
                (node, symlink_buffer, sizeof(symlink_buffer));
            if (error < 0) {
                inode_deref(node);
                return error;
            } else if (error == 0) {
                inode_deref(node);
                return -ENOENT;
            }

            /* Convert the symbolic link reference into an absolute pathname */
            /* TODO: This loses the tail after the symlink expansion! */
            pathname[posn] = '\0';
            error = inode_resolve_relative
                (pathname, CONFIG_PATH_MAX, posn, symlink_buffer);
            if (error < 0) {
                inode_deref(node);
                return error;
            }

            /* Restart path resolution from the root on down */
            posn = 1;
            inode_deref(node);
            node = inode_get_root();
            continue;
#else /* !CONFIG_SYMLINK */
            /* Symbolic links are not supported */
            (void)follow;
            inode_deref(node);
            return -EINVAL;
#endif /* !CONFIG_SYMLINK */
        }

        /* Advance to the next pathname component */
        if (is_last)
            break;
        else
            posn += namelen + 1;
    }

    /* Check the access permisssions on the node we found */
    error = inode_access(node, oflag_to_access_mode(oflag));
    if (error < 0) {
        inode_deref(node);
        return error;
    }

    /* Check that the expected type matches */
    if (((mode ^ node->mode) & S_IFMT) != 0) {
        if (S_ISDIR(mode)) {
            /* Expecting a directory, but this isn't one */
            inode_deref(node);
            return -ENOTDIR;
        } else if (S_ISDIR(node->mode)) {
            /* Entry is a directory, but we weren't expecting one */
            inode_deref(node);
            return -EISDIR;
        } else {
            /* Not the type of file that we expected */
            inode_deref(node);
            return -EACCES;
        }
    }

    /* Are we attempting to create the entry in exclusive mode? */
    if ((oflag & O_CREAT) != 0 && (oflag & O_EXCL) != 0) {
        inode_deref(node);
        return -EEXIST;
    }

    /* Return the referenced node to the caller */
    if (inode)
        *inode = node;
    else
        inode_deref(node);
    return 0;
}

ATTR_NOINLINE int inode_lookup_path
    (struct inode **inode, const char *pathname, int oflag,
     mode_t mode, u_char follow)
{
    /* Resolve the supplied pathname to an absolute path */
    int error = inode_path_to_abs(temp_path, sizeof(temp_path), pathname);
    if (error < 0) {
        return error;
    }

    /* Look up the path and/or create the new node */
    error = inode_lookup(inode, temp_path, oflag, mode, follow);
    if (error < 0) {
        return error;
    }

    /* Note: The buffer cache is left locked on success */
    return 0;
}

int inode_access(const struct inode *inode, int mode)
{
    if (mode == F_OK) {
        /* Checking for existence.  If we have a pointer to the inode,
         * then it exists! */
        return 0;
    } else {
        /* Use the effective uid and gid to check the permissions */
#if CONFIG_ACCESS_UID
        uid_t uid = current_proc->euid;
        uid_t gid = current_proc->egid;
        mode_t imode = inode->mode;
        if (uid == inode->uid) {
            imode >>= 6;
        } else if (gid == inode->gid) {
            imode >>= 3;
        }
#else
        /* Root is the only user, so only user checks matter */
        mode_t imode = inode->mode >> 6;
#endif
        if ((mode & imode & 7) == mode) {
            return 0;
        } else {
            return -EACCES;
        }
    }
}

time_t inode_get_mtime(void)
{
    /* TODO */
    return 0;
}

/* Default implementations of inode operations */

int inode_release_default(struct inode *inode)
{
    (void)inode;
    return 0;
}
