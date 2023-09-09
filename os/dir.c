/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <mosnix/syscall.h>
#include <mosnix/inode.h>
#include <mosnix/proc.h>
#include <mosnix/util.h>
#include <mosnix/config.h>
#include <errno.h>
#include <string.h>

/** Temporary pathname buffer, to keep it off the stack */
static char temp_path[CONFIG_PATH_MAX] ATTR_SECTION_NOINIT;

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

int sys_chdir(struct sys_chdir_s *args)
{
    /* Resolve the supplied pathname to an absolute path */
    int error = inode_path_to_abs(temp_path, sizeof(temp_path), args->path);
    if (error < 0)
        return error;

    /* TODO: validate the path against the filesystem */

    /* Replace the working directory in the current process */
    strcpy_constrained(current_proc->cwd, temp_path, sizeof(current_proc->cwd));
    return 0;
}

int sys_mkdir(struct sys_mkdir_s *args)
{
    /* TODO */
    (void)args;
    return -ENOSYS;
}

int sys_rmdir(struct sys_rmdir_s *args)
{
    /* TODO */
    (void)args;
    return -ENOSYS;
}

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

int sys_mknod(struct sys_mknod_s *args)
{
    /* TODO */
    (void)args;
    return -ENOSYS;
}

int sys_chmod(struct sys_chmod_s *args)
{
    /* TODO */
    (void)args;
    return -ENOSYS;
}

int sys_chown(struct sys_chown_s *args)
{
    /* TODO */
    (void)args;
    return -ENOSYS;
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

int sys_lstat(struct sys_lstat_s *args)
{
    /* TODO */
    (void)args;
    return -ENOSYS;
}

int sys_fstat(struct sys_fstat_s *args)
{
    /* TODO */
    (void)args;
    return -ENOSYS;
}

int sys_opendir(struct sys_opendir_s *args)
{
    /* TODO */
    (void)args;
    return -ENOSYS;
}

int sys_readdir(struct sys_readdir_s *args)
{
    /* TODO */
    (void)args;
    return -ENOSYS;
}
