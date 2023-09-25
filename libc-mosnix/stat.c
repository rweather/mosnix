/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <sys/stat.h>
#include <sys/syscall.h>

int stat(const char *path, struct stat *buf)
{
    return syscall(SYS_stat, path, buf);
}

int lstat(const char *path, struct stat *buf)
{
    return syscall(SYS_lstat, path, buf);
}

int fstat(int fd, struct stat *buf)
{
    return syscall(SYS_fstat, fd, buf);
}

mode_t umask(mode_t mask)
{
    return (mode_t)syscall(SYS_umask, mask);
}
