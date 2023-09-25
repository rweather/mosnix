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
 * @brief Maximum number of file descriptors in the per-process fd table.
 */
#ifndef CONFIG_PROC_FD_MAX
#define CONFIG_PROC_FD_MAX 16
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

/**
 * @brief Number of bytes of the 0x0100 page to allocate to each user
 * process as its 6502 return address stack.
 */
#ifndef CONFIG_RETURN_STACK_SIZE
#define CONFIG_RETURN_STACK_SIZE 64
#endif

/**
 * @brief Number of bytes of space to allocate to each user process
 * for its kernel data stack.
 */
#ifndef CONFIG_KERNEL_STACK_SIZE
#define CONFIG_KERNEL_STACK_SIZE 256
#endif

/**
 * @brief Maximum number of bytes in a filesystem path, including the
 * terminating NUL.
 */
#ifndef CONFIG_PATH_MAX
#define CONFIG_PATH_MAX 64
#endif

/**
 * @brief Maximum depth for resolving symbolic links before concluding
 * that we probably have a loop.
 */
#ifndef CONFIG_MAX_SYMLINK_DEPTH
#define CONFIG_MAX_SYMLINK_DEPTH 64
#endif

/**
 * @def CONFIG_SYMLINK
 * @brief Define to 1 to allow the use of symbolic links in the kernel.
 */
#ifndef CONFIG_SYMLINK
#define CONFIG_SYMLINK 0
#endif

/**
 * @def CONFIG_ACCESS_UID
 * @brief Define to 1 to allow the use of uid and gid access checks.
 *
 * If this is defined to 0, then root is the only user in the system.
 */
#ifndef CONFIG_ACCESS_UID
#define CONFIG_ACCESS_UID 0
#endif

#endif
