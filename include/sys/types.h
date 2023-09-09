/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#ifndef MOSNIX_SYS_TYPES_H
#define MOSNIX_SYS_TYPES_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned short ushort;
typedef unsigned int u_int;
typedef unsigned int uint;
typedef unsigned long u_long;
typedef unsigned long ulong;
typedef long long quad_t;
typedef unsigned long long u_quad_t;

typedef unsigned long blkcnt_t;
typedef unsigned int blksize_t;
typedef char *caddr_t;
typedef unsigned long clock_t;
typedef long daddr_t;
typedef unsigned int dev_t;
typedef unsigned short gid_t;
typedef unsigned short id_t;
typedef unsigned long ino_t;
typedef unsigned long long ino64_t;
typedef int key_t;
typedef unsigned short mode_t;
typedef unsigned short nlink_t;
typedef long off_t;
typedef long long loff_t;
typedef int pid_t;
typedef unsigned long sigset_t;
typedef int ssize_t;
typedef long suseconds_t;
typedef long time_t;
typedef unsigned short uid_t;
typedef unsigned long useconds_t;

#ifdef __cplusplus
}
#endif

#endif
