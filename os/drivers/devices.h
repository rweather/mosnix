/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#ifndef MOSNIX_DRIVERS_DEVICES_H
#define MOSNIX_DRIVERS_DEVICES_H

#include <mosnix/file.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opens a device by its identifier.
 *
 * @param[out] file The file descriptor to initialize for the device.
 *
 * @return Zero on success or a negative error code.
 */
int device_open(dev_t dev, struct file *file);

#ifdef __cplusplus
}
#endif

#endif
