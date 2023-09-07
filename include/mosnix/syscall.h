/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#ifndef MOSNIX_SYSCALL_H
#define MOSNIX_SYSCALL_H

#include <sys/types.h>
#include <sys/utsname.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated automatically */

#define SYS_ATTR extern __attribute__((interrupt, no_isr))

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
    const char *path;
    int flags;
    unsigned int mode;
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

struct sys_fcntl_s {
    int fd;
    int cmd;
    int value;
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

struct sys_uname_s {
    struct utsname *buf;
};

/*   0 */ SYS_ATTR int sys_read(struct sys_read_s *args);
/*   1 */ SYS_ATTR int sys_write(struct sys_write_s *args);
/*   2 */ SYS_ATTR int sys_open(struct sys_open_s *args);
/*   3 */ SYS_ATTR int sys_close(struct sys_close_s *args);
/*   4 */ SYS_ATTR int sys_lseek(struct sys_lseek_s *args);
/*   5 */ SYS_ATTR int sys_fcntl(struct sys_fcntl_s *args);
/*  20 */ SYS_ATTR int sys_getpid(void);
/*  21 */ SYS_ATTR int sys_getppid(void);
/*  22 */ SYS_ATTR void sys_exit(struct sys_exit_s *args);
/*  23 */ SYS_ATTR int sys_sched_yield(void);
/*  30 */ SYS_ATTR int sys_getuid(void);
/*  31 */ SYS_ATTR int sys_geteuid(void);
/*  32 */ SYS_ATTR int sys_setuid(struct sys_setuid_s *args);
/*  33 */ SYS_ATTR int sys_seteuid(struct sys_seteuid_s *args);
/*  34 */ SYS_ATTR int sys_getgid(void);
/*  35 */ SYS_ATTR int sys_getegid(void);
/*  36 */ SYS_ATTR int sys_setgid(struct sys_setgid_s *args);
/*  37 */ SYS_ATTR int sys_setegid(struct sys_setegid_s *args);
/* 100 */ SYS_ATTR int sys_uname(struct sys_uname_s *args);
/* N/A */ SYS_ATTR int sys_notimp(void);

#ifdef __cplusplus
}
#endif

#endif
