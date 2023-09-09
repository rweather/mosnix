/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include "ramfs.h"
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

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
            /* TODO */
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
        /* TODO */
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

struct inode_operations const ramfs_operations = {
    .release = ramfs_release,
    .lookup = ramfs_lookup,
    .readlink = ramfs_readlink
};

void ramfs_init(void)
{
    struct ramfs_dirent *dot;
    struct ramfs_dirent *dotdot;

    /* Lock the buffer cache while we do this */
    kmalloc_buf_lock();

    /* Create the root inode */
    root = inode_alloc(&ramfs_operations);
    root->mode = S_IRUSR | S_IWUSR | S_IXUSR | S_IFDIR |
                 S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH;
    inode_ref(root); /* Reference for the "." link */
    inode_ref(root); /* Reference for the ".." link */

    /* Create the "." and ".." links back to the root inode.  We arrange it
     * so that ".." from the root directory will go to the root directory.
     * The circular references here make it impossible to rmdir the root. */
    dot = kmalloc_buf_alloc();
    dotdot = kmalloc_buf_alloc();
    dot->namelen = 1;
    dot->name[0] = '.';
    dot->inode = root;
    dot->next = dotdot;
    dotdot->namelen = 2;
    dotdot->name[0] = '.';
    dotdot->name[1] = '.';
    dot->inode = root;
    root->ramfs_dir = dot;

    /* Unlock the buffer cache */
    kmalloc_buf_unlock();
}

struct inode *inode_get_root(void)
{
    inode_ref(root);
    return root;
}
