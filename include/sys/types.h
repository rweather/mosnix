/*
 * Copyright (C) 2023 Southern Storm Software, Pty Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
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

typedef unsigned long long blkcnt_t;
typedef unsigned int blksize_t;
typedef char *caddr_t;
typedef long long clock_t;
typedef long daddr_t;
typedef unsigned int dev_t;
typedef unsigned int gid_t;
typedef unsigned int id_t;
typedef unsigned long long ino_t;
typedef int key_t;
typedef unsigned long mode_t;
typedef unsigned int nlink_t;
typedef long long off_t;
typedef off_t loff_t;
typedef int pid_t;
typedef unsigned long sigset_t;
typedef int ssize_t;
typedef long suseconds_t;
typedef long long time_t;
typedef unsigned long useconds_t;

#ifdef __cplusplus
}
#endif

#endif
