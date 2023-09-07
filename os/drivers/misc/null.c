/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include "misc.h"
#include <string.h>
#include <errno.h>

static ssize_t dev_null_read(struct file *file, void *data, size_t size)
{
    /* Always report EOF */
    (void)file;
    (void)data;
    (void)size;
    return 0;
}

static ssize_t dev_zero_read(struct file *file, void *data, size_t size)
{
    /* Always fill the buffer with zeroes */
    (void)file;
    memset(data, 0, size);
    return size;
}

static ssize_t dev_null_write(struct file *file, const void *data, size_t size)
{
    /* Always throw the data away */
    (void)file;
    (void)data;
    return size;
}

static ssize_t dev_full_write(struct file *file, const void *data, size_t size)
{
    /* Always throw the data away and report no space left on device */
    (void)file;
    (void)data;
    (void)size;
    return -ENOSPC;
}

static struct file_operations const dev_null_operations = {
    .close = file_close_default,
    .read = dev_null_read,
    .write = dev_null_write,
    .lseek = file_lseek_default,
    .ioctl = file_ioctl_default
};

static struct file_operations const dev_zero_operations = {
    .close = file_close_default,
    .read = dev_zero_read,
    .write = dev_null_write,
    .lseek = file_lseek_default,
    .ioctl = file_ioctl_default
};

static struct file_operations const dev_full_operations = {
    .close = file_close_default,
    .read = dev_zero_read,
    .write = dev_full_write,
    .lseek = file_lseek_default,
    .ioctl = file_ioctl_default
};

int open_dev_null(struct file *file)
{
    file->op = &dev_null_operations;
    return 0;
}

int open_dev_zero(struct file *file)
{
    file->op = &dev_zero_operations;
    return 0;
}

int open_dev_full(struct file *file)
{
    file->op = &dev_full_operations;
    return 0;
}
