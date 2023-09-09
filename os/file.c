/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <mosnix/syscall.h>
#include <mosnix/file.h>
#include <mosnix/proc.h>
#include <bits/fcntl.h>
#include <errno.h>

/* Common file operations */

static struct file global_fd[CONFIG_FD_MAX] ATTR_SECTION_NOINIT;

void file_init(void)
{
    unsigned index;
    struct file *file = global_fd;
    for (index = 0; index < CONFIG_FD_MAX; ++index, ++file)
        file->count = 0;
}

ATTR_NOINLINE int file_deref(struct file *file)
{
    --(file->count);
    if (file->count <= 0)
        return file->op->close(file);
    else
        return 0;
}

ATTR_NOINLINE struct file *file_get(int fd)
{
    if (fd < 0 || fd >= CONFIG_PROC_FD_MAX)
        return 0;
    return current_proc->fd[fd];
}

ATTR_NOINLINE struct file *file_new(int flags, mode_t mode)
{
    unsigned index;
    struct file *file = global_fd;
    for (index = 0; index < CONFIG_FD_MAX; ++index, ++file) {
        if (file->count == 0) {
            file->count = 1;
            file->flags = flags;
            file->mode = mode;
            file->op = &file_default_operations;
            return file;
        }
    }
    return 0;
}

/* System call interface */

int sys_open(struct sys_open_s *args)
{
    /* TODO */
    (void)args;
    return -EPERM;
}

int sys_close(struct sys_close_s *args)
{
    /* Get the file descriptor structure */
    struct file *file = file_get(args->fd);
    if (!file)
        return -EBADF;

    /* Remove the file descriptor from the current process */
    current_proc->fd[args->fd] = 0;

    /* Dereference the file descriptor, which will actually close it
     * if this is the last copy in use by any process. */
    return file_deref(file);
}

int sys_read(struct sys_read_s *args)
{
    ssize_t result;

    /* Get the file descriptor structure */
    struct file *file = file_get(args->fd);
    if (!file)
        return -EBADF;

    /* The buffer pointer must not be NULL if the read size is non-zero  */
    if (!(args->data) && args->size > 0)
        return -EFAULT;

    /* Add a reference to the file in case the read function blocks */
    file_ref(file);

    /* Perform the read using the back-end implementation */
    result = file->op->read(file, args->data, args->size);

    /* Dereference the file and return */
    file_deref(file);
    return result;
}

int sys_write(struct sys_write_s *args)
{
    ssize_t result;

    /* Get the file descriptor structure */
    struct file *file = file_get(args->fd);
    if (!file)
        return -EBADF;

    /* The buffer pointer must not be NULL if the write size is non-zero  */
    if (!(args->data) && args->size > 0)
        return -EFAULT;

    /* Add a reference to the file in case the write function blocks */
    file_ref(file);

    /* Perform the write using the back-end implementation */
    result = file->op->write(file, args->data, args->size);

    /* Dereference the file and return */
    file_deref(file);
    return result;
}

int sys_lseek(struct sys_lseek_s *args)
{
    off_t result;

    /* Get the file descriptor structure */
    struct file *file = file_get(args->fd);
    if (!file)
        return -EBADF;

    /* Validate the "whence" value */
    /* TODO */

    /* Add a reference to the file in case the lseek function blocks */
    file_ref(file);

    /* Perform the lseek using the back-end implementation */
    result = file->op->lseek(file, args->offset, args->whence);

    /* Dereference the file and return */
    file_deref(file);
    return result;
}

static int sys_dup_scan(int oldfd, int newfd)
{
    struct file **fd;

    /* Get the file descriptor to be duplicated */
    struct file *file = file_get(oldfd);
    if (!file)
        return -EBADF;

    /* Validate the starting point for the scan */
    if (newfd < 0 || newfd >= CONFIG_PROC_FD_MAX)
        return -EBADF;

    /* Scan for a free slot in the process's file descriptor table */
    fd = current_proc->fd + newfd;
    while (newfd < CONFIG_PROC_FD_MAX) {
        if (!(*fd)) {
            /* Add a reference to the file descriptor and duplicate it */
            file_ref(file);
            *fd = file;

            /* Reset FD_CLOEXEC on the new file descriptor */
            current_proc->cloexec[newfd] = 0;
            return newfd;
        }
        ++newfd;
        ++fd;
    }
    return -EMFILE;
}

int sys_dup(struct sys_dup_s *args)
{
    return sys_dup_scan(args->oldfd, 0);
}

int sys_dup2(struct sys_dup2_s *args)
{
    int newfd = args->newfd;
    struct file *file;
    struct file *file2;

    /* Get the file descriptor to be duplicated */
    file = file_get(args->oldfd);
    if (!file)
        return -EBADF;

    /* Validate the new file descriptor */
    if (newfd < 0 || newfd >= CONFIG_PROC_FD_MAX)
        return -EBADF;

    /* If the new file descriptor is the same as the old, nothing to do */
    if (newfd == args->oldfd)
        return newfd;

    /* Add a reference to the descriptor to be duplicated */
    file_ref(file);

    /* Remove a reference from the descriptor that is being replaced,
     * which may result in it being closed. */
    file2 = file_get(newfd);
    if (file2)
        file_deref(file2);

    /* Replace the descriptor for newfd */
    current_proc->fd[newfd] = file;

    /* Reset the FD_CLOEXEC flag on the new file descriptor */
    current_proc->cloexec[newfd] = 0;
    return newfd;
}

int sys_fcntl(struct sys_fcntl_s *args)
{
    /* Get the file descriptor structure */
    struct file *file = file_get(args->fd);
    if (!file)
        return -EBADF;

    /* Determine what needs to be done */
    switch (args->cmd) {
    case F_DUPFD:
        /* Duplicate this file descriptor to a minimum new fd */
        return sys_dup_scan(args->fd, args->value);

    case F_GETFD:
        /* Get the state of the FD_CLOEXEC flag.  When a file descriptor is
         * duplicated, FD_CLOEXEC is reset in the duplicate so we need to
         * store this flag outside of the "struct file" object. */
        return current_proc->cloexec[args->fd];

    case F_SETFD:
        /* Set the state of the FD_CLOEXEC flag */
        current_proc->cloexec[args->fd] = (args->value & FD_CLOEXEC);
        break;

    case F_GETFL:
        /* Get the open flags on this file descriptor */
        return file->flags;

    case F_SETFL:
        /* The only flag we can change is O_NONBLOCK; ignore all others */
        if (args->value & O_NONBLOCK)
            file->flags |= O_NONBLOCK;
        else
            file->flags &= ~O_NONBLOCK;
        break;
    }
    return -EINVAL;
}

/* Default implementations of the file operations */

int file_close_default(struct file *file)
{
    (void)file;
    return 0;
}

ssize_t file_read_default(struct file *file, void *data, size_t size)
{
    (void)file;
    (void)data;
    (void)size;
    return -EINVAL;
}

ssize_t file_write_default(struct file *file, const void *data, size_t size)
{
    (void)file;
    (void)data;
    (void)size;
    return -EINVAL;
}

off_t file_lseek_default(struct file *file, off_t offset, int whence)
{
    (void)file;
    (void)offset;
    (void)whence;
    return -ESPIPE;
}

int file_ioctl_default(struct file *file, unsigned long request, void *args)
{
    (void)file;
    (void)request;
    (void)args;
    return -EINVAL;
}

const struct file_operations file_default_operations = {
    .close = file_close_default,
    .read = file_read_default,
    .write = file_write_default,
    .lseek = file_lseek_default,
    .ioctl = file_ioctl_default
};
