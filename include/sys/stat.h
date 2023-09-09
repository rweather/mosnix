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
#define S_ISCHR(mode)   (((mode) & S_IFMT) == S_IFCHR)
#define S_ISDIR(mode)   (((mode) & S_IFMT) == S_IFDIR)
#define S_ISBLK(mode)   (((mode) & S_IFMT) == S_IFBLK)
#define S_ISREG(mode)   (((mode) & S_IFMT) == S_IFREG)
#define S_ISLNK(mode)   (((mode) & S_IFMT) == S_IFLNK)
#define S_ISSOCK(mode)  (((mode) & S_IFMT) == S_IFSOCK)

/* Status information */
struct stat
{
    dev_t       st_dev;
    ino_t       st_ino;
    mode_t      st_mode;
    nlink_t     st_nlink;
    uid_t       st_uid;
    gid_t       st_gid;
    dev_t       st_rdev;
    off_t       st_size;
    blksize_t   st_blksize;
    blkcnt_t    st_blocks;
};

int stat(const char *path, struct stat *buf);
int lstat(const char *path, struct stat *buf);
int fstat(int fd, struct stat *buf);
int mkdir(const char *path, mode_t mode);

#ifdef __cplusplus
}
#endif

#endif
