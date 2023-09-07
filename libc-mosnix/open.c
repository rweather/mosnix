/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <stdarg.h>

int open(const char *path, int oflag, ...)
{
    unsigned int mode;
    if (oflag & O_CREAT) {
        va_list va;
        va_start(va, oflag);
        mode = va_arg(va, unsigned int);
        va_end(va);
    } else {
        mode = 0644;
    }
    return syscall(SYS_open, path, oflag, mode);
}

int creat(const char *path, mode_t mode)
{
    int oflag = O_WRONLY | O_CREAT | O_TRUNC;
    return syscall(SYS_open, path, oflag, (unsigned int)mode);
}
