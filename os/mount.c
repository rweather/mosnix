/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <mosnix/syscall.h>
#include "fs/fat/fatfs.h"
#include "fs/ram/ramfs.h"
#include <sys/mount.h>
#include <errno.h>
#include <string.h>

int sys_mount(struct sys_mount_s *args)
{
    struct inode *dir;
    int error;

    /* Validate the parameters */
    if (!(args->source)) {
        return -EFAULT;
    }
    if (!(args->filesystemtype)) {
        return -EFAULT;
    }

    /* Check for the sources and filesystem types we know about */
    if (strcmp(args->source, "/dev/mmcblk0") != 0 || !fatfs_have_sd()) {
        return -ENOTBLK;
    }
    if (strcmp(args->filesystemtype, "vfat") != 0) {
        return -ENODEV;
    }

    /* Non-POSIX behaviour: "target" is allowed to be NULL to check
     * if we have the block device before attempting to mount it.
     *
     * This is a stop-gap measure until we can support opening block
     * devices directly from user-space applications. */
    if (!args->target) {
        return 0;
    }

    /* Resolve the target path to the directory to mount on */
    error = inode_lookup_path(&dir, args->target, O_EXEC, S_IFDIR, 1);
    if (error < 0) {
        return error;
    }

    /* Target must be in the RAM filesystem and a directory */
    if (dir->op != &ramfs_operations) {
        inode_deref(dir);
        return -EINVAL;
    }
    if (!S_ISDIR(dir->mode)) {
        inode_deref(dir);
        return -ENOTDIR;
    }

    /* Target must not already be mounted, unless we are remounting */
    if (dir->mode & S_ISVTX) {
        inode_deref(dir);

        /* Since there is currently only one type of filesystem that
         * can be mounted, we just assume that remounts work.
         *
         * TODO: Remounting may change the mount point to or from read-only,
         * so we should probably update the flags. */
        if (args->mountflags & MS_REMOUNT)
            return 0;
        else
            return -EBUSY;
    }

    /* Perform the mount operation */
    error = fatfs_mount_sd(dir);
    inode_deref(dir);
    return error;
}

int sys_umount(struct sys_umount_s *args)
{
    /* Cannot unmount filesystems at present */
    (void)args;
    return -EPERM;
}
