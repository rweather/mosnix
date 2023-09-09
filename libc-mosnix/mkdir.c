/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <sys/stat.h>
#include <sys/syscall.h>

int mkdir(const char *path, mode_t mode)
{
    return syscall(SYS_mkdir, path, mode);
}
