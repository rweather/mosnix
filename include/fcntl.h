/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#ifndef MOSNIX_FCNTL_H
#define MOSNIX_FCNTL_H

#include <sys/types.h>
#include <bits/fcntl.h>
#include <bits/stat.h>

#ifdef __cplusplus
extern "C" {
#endif

extern int fcntl(int fd, int cmd, ...);
extern int open(const char *path, int oflag, ...);
extern int creat(const char *path, mode_t mode);

#ifdef __cplusplus
}
#endif

#endif
