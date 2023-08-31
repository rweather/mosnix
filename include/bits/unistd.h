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

#ifndef MOSNIX_BITS_UNISTD_H
#define MOSNIX_BITS_UNISTD_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated automatically */

extern ssize_t read(int fd, void *data, size_t size);
extern ssize_t write(int fd, const void *data, size_t size);
extern int open(const char *filename, int flags, int mode);
extern int close(int fd);
extern off_t lseek(int fd, off_t offset, int whence);
extern pid_t getpid(void);
extern void _exit(int status);
extern uid_t getuid(void);
extern uid_t geteuid(void);
extern int setuid(uid_t uid);
extern int seteuid(uid_t uid);
extern gid_t getgid(void);
extern gid_t getegid(void);
extern int setgid(gid_t uid);
extern int setegid(gid_t uid);

#ifdef __cplusplus
}
#endif

#endif
