/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#ifndef MOSNIX_SYS_SYSMACROS_H
#define MOSNIX_SYS_SYSMACROS_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Make a device number */
#define makedev(maj, min) ((((dev_t)(maj)) << 8) | ((dev_t)(min)))

/* Extract the major number of a device number */
#define major(dev) ((u_char)((dev) >> 8))

/* Extract the minor number of a device number */
#define minor(dev) ((u_char)(dev))

#ifdef __cplusplus
}
#endif

#endif
