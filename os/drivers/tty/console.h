/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#ifndef MOSNIX_DRIVERS_TTY_CONSOLE_H
#define MOSNIX_DRIVERS_TTY_CONSOLE_H

#include <mosnix/file.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opens the primary console tty with a default line discipline.
 *
 * @param[out] file The file descriptor to initialize for the console.
 *
 * @return Zero on success or a negative error code.
 */
int open_dev_tty0(struct file *file);

/**
 * @brief Opens the primary console tty in raw mode.
 *
 * @param[out] file The file descriptor to initialize for the console.
 *
 * @return Zero on success or a negative error code.
 */
int open_dev_console(struct file *file);

#ifdef __cplusplus
}
#endif

#endif
