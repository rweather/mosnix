/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include "ramfs.h"
#include <mosnix/proc.h>
#include <mosnix/config.h>
#include <mosnix/devices.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#ifndef RAMFS_DOT_DIRS
/* Define to 1 to define the "." and ".." directories, or 0 to save space */
#define RAMFS_DOT_DIRS 0
#endif

static struct inode *root;

static int ramfs_is_dot(const struct ramfs_dirent *dirent)
{
    if (dirent->namelen != 1)
        return 0;
    return dirent->name[0] == '.';
}

static int ramfs_is_dot_dot(const struct ramfs_dirent *dirent)
{
    if (dirent->namelen != 2)
        return 0;
    if (dirent->name[0] != '.')
        return 0;
    return dirent->name[1] == '.';
}

static int ramfs_release(struct inode *inode)
{
    if (S_ISDIR(inode->mode)) {
        /* Free the contents of a directory node */
        struct ramfs_dirent *current;
        struct ramfs_dirent *next;
        current = inode->ramfs_dir;
        while (current != 0) {
            next = current->next;
            if (!ramfs_is_dot(current) && !ramfs_is_dot_dot(current)) {
                inode_deref(current->inode);
            }
            kmalloc_buf_free(current);
            current = next;
        }
        if (inode->mode & S_ISVTX) {
            /* This directory is also a mount point, so unmount it */
            if (inode->ramfs_mount) {
                inode_deref(inode->ramfs_mount);
            }
        }
    } else if (S_ISREG(inode->mode) || S_ISLNK(inode->mode)) {
        /* Free the data for a regular file or symbolic link */
        struct ramfs_data *current;
        struct ramfs_data *next;
        current = inode->ramfs_file.data;
        while (current != 0) {
            next = current->next;
            kmalloc_buf_free(current);
            current = next;
        }
    }
    return 0;
}

static int ramfs_lookup
    (struct inode **inode, struct inode *dir, const char *name, size_t namelen)
{
    struct ramfs_dirent *dirent;

    /* The inode must be a directory or this doesn't make sense */
    if (!S_ISDIR(dir->mode)) {
        return -ENOTDIR;
    }

    /* If the VTX bit is set, then this is actually a mount point and
     * we need to pass the lookup request onto the mounted filesystem. */
    if (dir->mode & S_ISVTX) {
        if (dir->ramfs_mount) {
            return dir->ramfs_mount->op->lookup
                (inode, dir->ramfs_mount, name, namelen);
        }
        return -ENOENT;
    }

    /* Search the directory for a match on the name */
    dirent = dir->ramfs_dir;
    while (dirent) {
        if (dirent->namelen == namelen) {
            if (!memcmp(dirent->name, name, namelen)) {
                struct inode *found = dirent->inode;
                if (found) {
                    inode_ref(found);
                    *inode = found;
                    return 0;
                } else {
                    return -EPERM;
                }
            }
        }
        dirent = dirent->next;
    }
    return -ENOENT;
}

static ssize_t ramfs_dir_read(struct file *file, void *data, size_t size)
{
    struct dirent *out = (struct dirent *)data;
    struct ramfs_dirent *entry;
    unsigned posn, count;
    unsigned char namelen;
    mode_t mode;
    ssize_t result;

    /* Seek to the correct entry.  It is possible that the directory
     * has been modified since last we looked at it, so we always seek
     * from the beginning every time.  Consider more efficient ways
     * to do this later. */
    entry = file->inode->ramfs_dir;
    posn = (unsigned)(file->posn);
    while (entry && posn > 0) {
        entry = entry->next;
        --posn;
    }

    /* Read entries from the directory into the supplied buffer */
    result = 0;
    count = 0;
    while (entry && size >= sizeof(struct dirent)) {
        mode = entry->inode->mode;
        out->d_type = mode >> 12;
        out->d_mode_np = mode;
        switch (mode & S_IFMT) {
        case S_IFREG:
        case S_IFLNK:
            out->d_ino = entry->inode->ramfs_file.size;
            break;
        case S_IFCHR:
        case S_IFBLK:
            out->d_ino = entry->inode->device;
            break;
        default:
            out->d_ino = 0;
            break;
        }
        out->d_mtime_np = entry->inode->mtime;
        namelen = entry->namelen;
        memcpy(out->d_name, entry->name, namelen);
        memset(out->d_name + namelen, 0, DIRENT_NAME_MAX - namelen);
        result += sizeof(struct dirent);
        size -= sizeof(struct dirent);
        entry = entry->next;
        ++count;
        ++out;
    }
    file->posn += count;
    return result;
}

static struct file_operations const ramfs_dir_operations = {
    .close = file_close_default,
    .read = ramfs_dir_read,
    .write = file_write_default,
    file_op_lseek_default
};

static int ramfs_open(struct file *file)
{
    struct inode *inode;
    int result;
    switch (file->mode & S_IFMT) {
    case S_IFDIR:
        /* Open a directory for reading */
        if (file->mode & S_ISVTX) {
            /* This directory is mounted, so pass the request onto the
             * underlying filesystem that is mounted here. */
            inode = file->inode->ramfs_mount;
            inode_ref(inode);
            inode_deref(file->inode);
            file->inode = inode;
            file->mode = inode->mode;
            result = inode->op->open(file);
        } else {
            file->op = &ramfs_dir_operations;
            result = 0;
        }
        break;

    case S_IFREG:
        /* Open a regular file for reading or writing */
        /* TODO */
        result = 0;
        break;

    case S_IFCHR:
        /* Open a character special device */
        inode = file->inode;
        file->inode = NULL;
        result = char_device_open(inode->device, file);
        inode_deref(inode);
        break;

    default:
        /* Cannot open this type of file.  In the case of block special
         * devices, they should be mounted instead of opened. */
        result = -EACCES;
        break;
    }
    return result;
}

static int ramfs_mknod
    (struct inode **inode, struct inode *dir,
     const char *name, size_t namelen, mode_t mode)
{
    struct ramfs_dirent *dirent;
    struct ramfs_dirent **ptr;
    struct inode *child;

    /* Directory check on the parent, just in case */
    if (!S_ISDIR(dir->mode)) {
        return -ENOTDIR;
    }

    /* Check the length of the name */
    if (namelen > RAMFS_MAX_NAME) {
        return -ENAMETOOLONG;
    }

    /* Allocate a new directory entry for the child */
    dirent = kmalloc_buf_alloc();
    if (!dirent) {
        return -ENOMEM;
    }

    /* Allocate a new inode for the child */
    child = inode_alloc(&ramfs_operations);
    if (!child) {
        kmalloc_buf_free(dirent);
        return -ENOMEM;
    }
    child->mode = mode;
#if CONFIG_ACCESS_UID
    child->uid = current_proc->euid;
    child->gid = current_proc->egid;
#endif
    child->mtime = inode_get_mtime();

    /* Populate the directory entry */
    dirent->namelen = namelen;
    memcpy(dirent->name, name, namelen);
    dirent->inode = child;

#if RAMFS_DOT_DIRS
    /* If the child is a directory, also create the "." and ".." entries */
    if (S_ISDIR(mode)) {
        struct ramfs_dirent *dot;
        struct ramfs_dirent *dotdot;
        dot = kmalloc_buf_alloc();
        if (!dot) {
            inode_deref(child);
            kmalloc_buf_free(dirent);
            return -ENOMEM;
        }
        dotdot = kmalloc_buf_alloc();
        if (!dotdot) {
            inode_deref(child);
            kmalloc_buf_free(dot);
            kmalloc_buf_free(dirent);
            return -ENOMEM;
        }
        dot->namelen = 1;
        dot->name[0] = '.';
        dot->inode = child;
        dot->next = dotdot;
        dotdot->namelen = 2;
        dotdot->name[0] = '.';
        dotdot->name[1] = '.';
        dotdot->inode = dir;
        child->ramfs_dir = dot;
    }
#endif

    /* Add the new directory entry to the end of the parent directory */
    ptr = &(dir->ramfs_dir);
    while (*ptr != NULL) {
        ptr = &((*ptr)->next);
    }
    *ptr = dirent;

    /* Add a reference to the child and return it to the caller */
    if (inode) {
        inode_ref(child);
        *inode = child;
    }
    return 0;
}

#if CONFIG_SYMLINK

static ssize_t ramfs_readlink(struct inode *inode, char *buf, size_t size)
{
    const struct ramfs_data *data;
    size_t result;

    /* Validate the parameters */
    if (!buf)
        return -EFAULT;
    if (!size)
        return -EINVAL;
    if (!S_ISLNK(inode->mode))
        return -EINVAL;

    /* Validate the size */
    if (size > inode->ramfs_file.size) {
        size = inode->ramfs_file.size;
    }

    /* Copy the contents of the link up to the specified size */
    result = 0;
    data = inode->ramfs_file.data;
    while (result < size && data) {
        size_t len = size - result;
        if (len >= sizeof(data->data)) {
            memcpy(buf + result, data->data, sizeof(data->data));
            result += sizeof(data->data);
        } else {
            memcpy(buf + result, data->data, len);
            result += len;
            break;
        }
        data = data->next;
    }
    return (ssize_t)result;
}

#endif /* CONFIG_SYMLINK */

struct inode_operations const ramfs_operations = {
    .release = ramfs_release,
    .lookup = ramfs_lookup,
    .open = ramfs_open,
    .mknod = ramfs_mknod,
#if CONFIG_SYMLINK
    .readlink = ramfs_readlink,
#endif
};

void ramfs_init(void)
{
    /* Create the root inode */
    root = inode_alloc(&ramfs_operations);
    root->mode = S_IRUSR | S_IWUSR | S_IXUSR | S_IFDIR |
                 S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH;
    inode_ref(root); /* Reference for the "." link */
    inode_ref(root); /* Reference for the ".." link */

#if RAMFS_DOT_DIRS
    /* Create the "." and ".." links back to the root inode.  We arrange it
     * so that ".." from the root directory will go to the root directory.
     * The circular references here make it impossible to rmdir the root. */
    struct ramfs_dirent *dot;
    struct ramfs_dirent *dotdot;
    dot = kmalloc_buf_alloc();
    dotdot = kmalloc_buf_alloc();
    dot->namelen = 1;
    dot->name[0] = '.';
    dot->inode = root;
    dot->next = dotdot;
    dotdot->namelen = 2;
    dotdot->name[0] = '.';
    dotdot->name[1] = '.';
    dotdot->inode = root;
    root->ramfs_dir = dot;
#endif
}

struct inode *inode_get_root(void)
{
    inode_ref(root);
    return root;
}
