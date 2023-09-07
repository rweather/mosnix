/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include "devices.h"
#include "misc/misc.h"
#include "tty/console.h"
#include <mosnix/devnum.h>
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

/** Table of all known devices. */
static struct device_info const devices[] = {
    {DEV_NULL,          open_dev_null},
    {DEV_ZERO,          open_dev_zero},
    {DEV_FULL,          open_dev_full},
    {0,                 0},
};

int device_open(dev_t dev, struct file *file)
{
    const struct device_info *info;
    for (info = devices; info->dev; ++info) {
        if (info->dev == dev)
            return info->open(file);
    }
    return -ENXIO;
}
