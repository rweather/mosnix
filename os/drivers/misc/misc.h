/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#ifndef MOSNIX_DRIVERS_MISC_MISC_H
#define MOSNIX_DRIVERS_MISC_MISC_H

#include <mosnix/file.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opens the /dev/null character device.
 *
 * @param[out] file The file descriptor to initialize for the device.
 *
 * @return Zero on success or a negative error code.
 */
int open_dev_null(struct file *file);

/**
 * @brief Opens the /dev/zero character device.
 *
 * @param[out] file The file descriptor to initialize for the device.
 *
 * @return Zero on success or a negative error code.
 */
int open_dev_zero(struct file *file);

/**
 * @brief Opens the /dev/full character device.
 *
 * @param[out] file The file descriptor to initialize for the device.
 *
 * @return Zero on success or a negative error code.
 */
int open_dev_full(struct file *file);

#ifdef __cplusplus
}
#endif

#endif
