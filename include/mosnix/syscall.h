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
#include <sys/stat.h>
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

struct sys_dup_s {
    int oldfd;
};

struct sys_dup2_s {
    int oldfd;
    int newfd;
};

struct sys_fstat_s {
    int fd;
    struct stat *statbuf;
};

struct sys_getcwd_s {
    char *buf;
    size_t size;
    char* *result;
};

struct sys_chdir_s {
    const char *path;
};

struct sys_mkdir_s {
    const char *path;
    mode_t mode;
};

struct sys_rmdir_s {
    const char *path;
};

struct sys_symlink_s {
    const char *target;
    const char *path;
};

struct sys_readlink_s {
    const char *target;
    char *buf;
    size_t size;
};

struct sys_mknod_s {
    const char *path;
    mode_t mode;
    dev_t dev;
};

struct sys_chmod_s {
    const char *path;
    mode_t mode;
};

struct sys_chown_s {
    const char *path;
    uid_t owner;
    gid_t group;
};

struct sys_unlink_s {
    const char *path;
};

struct sys_stat_s {
    const char *path;
    struct stat *statbuf;
};

struct sys_lstat_s {
    const char *path;
    struct stat *statbuf;
};

struct sys_opendir_s {
    const char *path;
};

struct sys_readdir_s {
    int fd;
    void *data;
    size_t size;
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
/*   6 */ SYS_ATTR int sys_dup(struct sys_dup_s *args);
/*   7 */ SYS_ATTR int sys_dup2(struct sys_dup2_s *args);
/*   8 */ SYS_ATTR int sys_fstat(struct sys_fstat_s *args);
/*  20 */ SYS_ATTR int sys_getcwd(struct sys_getcwd_s *args);
/*  21 */ SYS_ATTR int sys_chdir(struct sys_chdir_s *args);
/*  22 */ SYS_ATTR int sys_mkdir(struct sys_mkdir_s *args);
/*  23 */ SYS_ATTR int sys_rmdir(struct sys_rmdir_s *args);
/*  24 */ SYS_ATTR int sys_symlink(struct sys_symlink_s *args);
/*  25 */ SYS_ATTR int sys_readlink(struct sys_readlink_s *args);
/*  26 */ SYS_ATTR int sys_mknod(struct sys_mknod_s *args);
/*  27 */ SYS_ATTR int sys_chmod(struct sys_chmod_s *args);
/*  28 */ SYS_ATTR int sys_chown(struct sys_chown_s *args);
/*  29 */ SYS_ATTR int sys_unlink(struct sys_unlink_s *args);
/*  30 */ SYS_ATTR int sys_stat(struct sys_stat_s *args);
/*  31 */ SYS_ATTR int sys_lstat(struct sys_lstat_s *args);
/*  32 */ SYS_ATTR int sys_opendir(struct sys_opendir_s *args);
/*  33 */ SYS_ATTR int sys_readdir(struct sys_readdir_s *args);
/*  50 */ SYS_ATTR int sys_getpid(void);
/*  51 */ SYS_ATTR int sys_getppid(void);
/*  52 */ SYS_ATTR void sys_exit(struct sys_exit_s *args);
/*  53 */ SYS_ATTR int sys_sched_yield(void);
/*  60 */ SYS_ATTR int sys_getuid(void);
/*  61 */ SYS_ATTR int sys_geteuid(void);
/*  62 */ SYS_ATTR int sys_setuid(struct sys_setuid_s *args);
/*  63 */ SYS_ATTR int sys_seteuid(struct sys_seteuid_s *args);
/*  64 */ SYS_ATTR int sys_getgid(void);
/*  65 */ SYS_ATTR int sys_getegid(void);
/*  66 */ SYS_ATTR int sys_setgid(struct sys_setgid_s *args);
/*  67 */ SYS_ATTR int sys_setegid(struct sys_setegid_s *args);
/* 100 */ SYS_ATTR int sys_uname(struct sys_uname_s *args);
/* N/A */ SYS_ATTR int sys_notimp(void);

#ifdef __cplusplus
}
#endif

#endif
