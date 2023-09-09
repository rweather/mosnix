/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
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
extern int close(int fd);
extern off_t lseek(int fd, off_t offset, int whence);
extern int dup(int oldfd);
extern int dup2(int oldfd, int newfd);
extern char* getcwd(char *buf, size_t size);
extern int chdir(const char *path);
extern int rmdir(const char *path);
extern int symlink(const char *target, const char *path);
extern ssize_t readlink(const char *target, char *buf, size_t size);
extern int mknod(const char *path, mode_t mode, dev_t dev);
extern int chmod(const char *path, mode_t mode);
extern int chown(const char *path, uid_t owner, gid_t group);
extern int unlink(const char *path);
extern pid_t getpid(void);
extern pid_t getppid(void);
extern void _exit(int status);
extern uid_t getuid(void);
extern uid_t geteuid(void);
extern int setuid(uid_t uid);
extern int seteuid(uid_t uid);
extern gid_t getgid(void);
extern gid_t getegid(void);
extern int setgid(gid_t gid);
extern int setegid(gid_t gid);

#ifdef __cplusplus
}
#endif

#endif
