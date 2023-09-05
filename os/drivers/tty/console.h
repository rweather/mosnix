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
 * @brief Opens the primary console tty.
 *
 * @param[out] file The file descriptor to initialize for the console.
 * The reference count will be initially 1.
 *
 * @return Zero on success or a negative error code.
 */
int open_console_tty(struct file *file);

#ifdef __cplusplus
}
#endif

#endif
