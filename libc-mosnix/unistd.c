/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <unistd.h>
#include <sys/syscall.h>

/* Generated automatically */

ssize_t read(int fd, void *data, size_t size)
{
    return syscall(SYS_read, fd, data, size);
}

ssize_t write(int fd, const void *data, size_t size)
{
    return syscall(SYS_write, fd, data, size);
}

int close(int fd)
{
    return syscall(SYS_close, fd);
}

off_t lseek(int fd, off_t offset, int whence)
{
    off_t result;
    if (syscall(SYS_lseek, fd, offset, whence, &result) == 0)
        return result;
    else
        return -1;
}

int dup(int oldfd)
{
    return syscall(SYS_dup, oldfd);
}

int dup2(int oldfd, int newfd)
{
    return syscall(SYS_dup2, oldfd, newfd);
}

char* getcwd(char *buf, size_t size)
{
    char* result;
    if (syscall(SYS_getcwd, buf, size, &result) == 0)
        return result;
    else
        return 0;
}

int chdir(const char *path)
{
    return syscall(SYS_chdir, path);
}

pid_t getpid(void)
{
    return syscall(SYS_getpid);
}

pid_t getppid(void)
{
    return syscall(SYS_getppid);
}

void _exit(int status)
{
    syscall(SYS_exit, status);
    while (1) ; /* stop compiler complaining about reachable unreachable code */
}

uid_t getuid(void)
{
    return syscall(SYS_getuid);
}

uid_t geteuid(void)
{
    return syscall(SYS_geteuid);
}

int setuid(uid_t uid)
{
    return syscall(SYS_setuid, uid);
}

int seteuid(uid_t uid)
{
    return syscall(SYS_seteuid, uid);
}

gid_t getgid(void)
{
    return syscall(SYS_getgid);
}

gid_t getegid(void)
{
    return syscall(SYS_getegid);
}

int setgid(gid_t gid)
{
    return syscall(SYS_setgid, gid);
}

int setegid(gid_t gid)
{
    return syscall(SYS_setegid, gid);
}

