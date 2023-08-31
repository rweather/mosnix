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

int open(const char *filename, int flags, int mode)
{
    return syscall(SYS_open, filename, flags, mode);
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

pid_t getpid(void)
{
    return syscall(SYS_getpid);
}

void _exit(int status)
{
    syscall(SYS_exit, status);
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

