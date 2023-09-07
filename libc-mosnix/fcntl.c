/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <fcntl.h>
#include <sys/syscall.h>
#include <stdarg.h>

int fcntl(int fd, int cmd, ...)
{
    int value;
    if (!(cmd & 1)) {
        va_list va;
        va_start(va, cmd);
        value = va_arg(va, int);
        va_end(va);
    } else {
        value = 0;
    }
    return syscall(SYS_fcntl, fd, cmd, value);
}
