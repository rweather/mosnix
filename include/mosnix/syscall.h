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

#ifndef MOSNIX_SYSCALL_H
#define MOSNIX_SYSCALL_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated automatically */

struct sys_read_s {
    int fd;
    void *data;
    size_t size;
};

struct sys_write_s {
    int fd;
    const void *data;
    size_t size;
};

struct sys_open_s {
    const char *filename;
    int flags;
    int mode;
};

struct sys_close_s {
    int fd;
};

struct sys_lseek_s {
    int fd;
    off_t offset;
    int whence;
    off_t *result;
};

struct sys_exit_s {
    int status;
};

struct sys_setuid_s {
    uid_t uid;
};

struct sys_seteuid_s {
    uid_t uid;
};

struct sys_setgid_s {
    gid_t gid;
};

struct sys_setegid_s {
    gid_t gid;
};

/*   0 */ extern int sys_read(struct sys_read_s *args);
/*   1 */ extern int sys_write(struct sys_write_s *args);
/*   2 */ extern int sys_open(struct sys_open_s *args);
/*   3 */ extern int sys_close(struct sys_close_s *args);
/*   4 */ extern int sys_lseek(struct sys_lseek_s *args);
/*  20 */ extern int sys_getpid(void);
/*  21 */ extern void sys_exit(struct sys_exit_s *args);
/*  30 */ extern int sys_getuid(void);
/*  31 */ extern int sys_geteuid(void);
/*  32 */ extern int sys_setuid(struct sys_setuid_s *args);
/*  33 */ extern int sys_seteuid(struct sys_seteuid_s *args);
/*  34 */ extern int sys_getgid(void);
/*  35 */ extern int sys_getegid(void);
/*  36 */ extern int sys_setgid(struct sys_setgid_s *args);
/*  37 */ extern int sys_setegid(struct sys_setegid_s *args);
/* N/A */ extern int sys_notimp(void);

#ifdef __cplusplus
}
#endif

#endif
