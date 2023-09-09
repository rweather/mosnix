/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <dirent.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

struct DIR_s
{
    int fd;
    struct dirent entry;
};

DIR *opendir(const char *path)
{
    DIR *dir;
    int fd;

    /* Try to open the directory */
    fd = syscall(SYS_opendir, path);
    if (fd < 0)
        return 0;

    /* Allocate memory for the directory control structure */
    dir = malloc(sizeof(struct DIR_s));
    if (!dir) {
        syscall(SYS_close, fd);
        errno = ENOMEM;
        return 0;
    }
    dir->fd = fd;
    return dir;
}

int closedir(DIR *dir)
{
    if (dir) {
        int fd = dir->fd;
        free(dir);
        return close(fd);
    } else {
        errno = EBADF;
        return -1;
    }
}

struct dirent *readdir(DIR *dir)
{
    ssize_t size;
    if (dir) {
        size = syscall(SYS_readdir, dir->fd, &(dir->entry), sizeof(dir->entry));
        if (size < 0)
            return 0;
        return &(dir->entry);
    } else {
        errno = EBADF;
        return 0;
    }
}
