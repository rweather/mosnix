/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <mosnix/devices.h>
#include <mosnix/devnum.h>
#include "misc/misc.h"
#include "tty/console.h"
#include <errno.h>

/**
 * @brief Prototype for a function that opens a device.
 *
 * @param[out] file The file descriptor to initialize for the device.
 *
 * @return Zero on success or a negative error code.
 */
typedef int (*device_open_func_t)(struct file *file);

/**
 * @brief Information about how to open a device.
 */
struct device_info
{
    dev_t dev;                  /**< Device number */
    device_open_func_t open;    /**< Function to use to open the device */
};

/** Table of all known character devices. */
static struct device_info const char_devices[] = {
    /* /dev/null, /dev/zero, and /dev/full */
    {DEV_NULL,          open_dev_null},
    {DEV_ZERO,          open_dev_zero},
    {DEV_FULL,          open_dev_full},

    /* There are multiple standard names for the console tty */
    {DEV_TTY,           open_console_tty},
    {DEV_TTY0,          open_console_tty},
    {DEV_CONSOLE,       open_console_tty},

    {0,                 0},
};

/** Table of all known block devices. */
static struct device_info const block_devices[] = {
    {0,                 0},
};

static int device_open
    (dev_t dev, struct file *file, const struct device_info *info)
{
    for (; info->dev; ++info) {
        if (info->dev == dev)
            return info->open(file);
    }
    return -ENXIO;
}

int char_device_open(dev_t dev, struct file *file)
{
    return device_open(dev, file, char_devices);
}

int block_device_open(dev_t dev, struct file *file)
{
    return device_open(dev, file, block_devices);
}
