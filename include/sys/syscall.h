/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#ifndef MOSNIX_SYS_SYSCALL_H
#define MOSNIX_SYS_SYSCALL_H

#include <bits/syscall.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Invoke a kernel system call.  If the result is negative,
 * then populate the "errno" variable and return -1. */
__attribute__((leaf)) int syscall(unsigned char number, ...);

#ifdef __cplusplus
}
#endif

#endif
