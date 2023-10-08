/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <mosnix/syscall.h>
#include <mosnix/inode.h>
#include <mosnix/file.h>
#include <mosnix/proc.h>
#include <mosnix/util.h>
#include <mosnix/config.h>
#include <bits/fcntl.h>
#include <errno.h>
#include <string.h>

int sys_notimp(void)
{
    return -ENOSYS;
}

int sys_getcwd(struct sys_getcwd_s *args)
{
    char *buf = args->buf;
    size_t size = args->size;

    /* Validate the parameters */
    if (!buf || !args->result)
        return -EFAULT;
    if (!size)
        return -EINVAL;

    /* Copy the working directory of the current process */
    *(args->result) = buf;
    strcpy_constrained(buf, current_proc->cwd, size);
    return 0;
}

extern char temp_path[CONFIG_PATH_MAX];

int sys_chdir(struct sys_chdir_s *args)
{
    int error = inode_lookup_path(NULL, args->path, O_EXEC, S_IFDIR, 1);
    if (error >= 0) {
        strcpy_constrained
            (current_proc->cwd, temp_path, sizeof(current_proc->cwd));
    }
    return error;
}

int sys_mkdir(struct sys_mkdir_s *args)
{
    struct inode *inode;
    mode_t mode;
    int error;

    /* Create the full mode */
    mode = S_IFDIR | (args->mode & ~(current_proc->umask) & 0777);

    /* Resolve and create the inode */
    error = inode_lookup_path(&inode, args->path, O_CREAT | O_EXCL, mode, 1);
    if (error >= 0) {
        inode_deref(inode);
    }
    return error;
}

int sys_rmdir(struct sys_rmdir_s *args)
{
    /* TODO */
    (void)args;
    return -ENOSYS;
}

#if CONFIG_SYMLINK

int sys_symlink(struct sys_symlink_s *args)
{
    /* TODO */
    (void)args;
    return -ENOSYS;
}

int sys_readlink(struct sys_readlink_s *args)
{
    /* TODO */
    (void)args;
    return -ENOSYS;
}

#endif /* !CONFIG_SYMLINK */

int sys_mknod(struct sys_mknod_s *args)
{
    struct inode *inode;
    mode_t mode;
    int error;

#if CONFIG_ACCESS_UID
    /* Must be root to do this */
    if (current_proc->uid != 0) {
        return -EPERM;
    }
#endif

    /* We only allow creation of character and block devices at the moment */
    mode = args->mode & ~(current_proc->umask);
    if (!S_ISCHR(mode) && !S_ISBLK(mode)) {
        return -EINVAL;
    }
    if (mode & (S_ISUID | S_ISGID | S_ISVTX)) {
        return -EINVAL;
    }

    /* Resolve and create the inode */
    error = inode_lookup_path(&inode, args->path, O_CREAT | O_EXCL, mode, 1);
    if (error >= 0) {
        /* Set the device number in the inode */
        inode->device = args->dev;
        inode_deref(inode);
    }
    return error;
}

int sys_unlink(struct sys_unlink_s *args)
{
    /* TODO */
    (void)args;
    return -ENOSYS;
}

int sys_stat(struct sys_stat_s *args)
{
    /* TODO */
    (void)args;
    return -ENOSYS;
}

#if CONFIG_SYMLINK

int sys_lstat(struct sys_lstat_s *args)
{
    /* TODO */
    (void)args;
    return -ENOSYS;
}

#else /* !CONFIG_SYMLINK */

/* If symbolic links are not supported, then lstat() is the same as stat() */
int sys_lstat(struct sys_lstat_s *args)  __attribute__((alias("sys_notimp")));

#endif /* !CONFIG_SYMLINK */

int sys_opendir(struct sys_opendir_s *args)
{
    return file_open(args->path, O_RDONLY, S_IFDIR);
}

int sys_umask(struct sys_umask_s *args)
{
    mode_t prev = current_proc->umask;
    current_proc->umask = args->mask & 0777;
    return prev;
}
