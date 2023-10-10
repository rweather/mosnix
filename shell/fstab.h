/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#ifndef SHELL_FSTAB_H
#define SHELL_FSTAB_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Adds an entry to the fstab for displaying mount points.
 *
 * @param[in] mountpoint Name of the mount point directory.
 * @param[in] device Device that is mounted.
 * @param[in] type Type of filesystem.
 * @param[in] options Mount options.
 *
 * The strings must be constant and must not be deallocated.
 */
void add_fstab
    (const char *mountpoint, const char *device, const char *type,
     const char *options);

#ifdef __cplusplus
}
#endif

#endif
