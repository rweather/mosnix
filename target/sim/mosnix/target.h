/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#ifndef MOSNIX_TARGET_SIM_H
#define MOSNIX_TARGET_SIM_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SYS_CLOCKS_PER_SEC 1000000UL

extern clock_t sys_clock(void);

#ifdef __cplusplus
}
#endif

#endif
