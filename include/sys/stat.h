/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#ifndef MOSNIX_SYS_STAT_H
#define MOSNIX_SYS_STAT_H

#include <sys/types.h>
#include <bits/stat.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Test for various types of filesystem node */
#define S_ISFIFO(mode)  (((mode) & S_IFMT) == S_IFIFO)
#define S_ISCHR         (((mode) & S_IFMT) == S_IFCHR)
#define S_ISDIR         (((mode) & S_IFMT) == S_IFDIR)
#define S_ISBLK         (((mode) & S_IFMT) == S_IFBLK)
#define S_ISREG         (((mode) & S_IFMT) == S_IFREG)
#define S_ISLNK         (((mode) & S_IFMT) == S_IFLNK)
#define S_ISSOCK        (((mode) & S_IFMT) == S_IFSOCK)

#ifdef __cplusplus
}
#endif

#endif
