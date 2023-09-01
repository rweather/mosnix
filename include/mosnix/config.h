/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#ifndef MOSNIX_CONFIG_H
#define MOSNIX_CONFIG_H

/* Configure the kernel's parameters */

/**
 * @brief Maximum number of processes in the global process table.
 *
 * Each process gets 32 zero page locations and 64 are reserved for the kernel.
 * This means that there is a maximum of 6 processes possible.
 */
#ifndef CONFIG_PROC_MAX
#define CONFIG_PROC_MAX 6
#endif

/**
 * @brief Maximum number of file descriptors in the global fd table.
 */
#ifndef CONFIG_FD_MAX
#define CONFIG_FD_MAX 64
#endif

/**
 * @brief Maximum bytes for argv command-line strings and the array overhead.
 */
#ifndef CONFIG_ARG_MAX
#define CONFIG_ARG_MAX 128
#endif

/**
 * @brief Maximum value for argc when counting command-line strings.
 *
 * Depending upon the size of each string in the argument array,
 * the actual maximum for a particular process may be less than this.
 */
#ifndef CONFIG_ARGC_MAX
#define CONFIG_ARGC_MAX 64
#endif

#endif
