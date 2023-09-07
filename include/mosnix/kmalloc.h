/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#ifndef MOSNIX_KMALLOC_H
#define MOSNIX_KMALLOC_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Size of an element in the kernel's "buffer cache" for storing
 * inodes, RAM filesystem data, and other housekeeping values.
 *
 * All buffers are of the same size to make memory housekeeping simpler.
 */
#define KMALLOC_BUF_SIZE 18

/**
 * @brief Initializes the kernel memory allocation system.
 */
void kmalloc_init(void);

/**
 * @brief Allocates a buffer from the kernel's buffer cache.
 *
 * @return The new buffer of exactly KMALLOC_BUF_SIZE bytes in length,
 * or NULL if the buffer cache has been exhausted.
 *
 * @note The returned pointer may be in a memory page that is currently
 * swapped out of the CPU's view of memory.  Callers must use
 * kmalloc_buf_lock() and kmalloc_buf_unlock() to gate access to buffers.
 */
void *kmalloc_buf_alloc(void);

/**
 * @brief Frees a buffer back to the kernel's buffer cache.
 *
 * @param[in] buf Points to the buffer to free.
 */
void kmalloc_buf_free(void *buf);

/**
 * @brief Locks access to the kernel buffer cache and ensures that
 * pointers to the cache are valid.
 *
 * Locks can be applied recursively.  The buffer cache is released when
 * the number of unlock calls matches the number of lock calls.
 *
 * While the buffer cache is locked, process memory may be inaccessible.
 *
 * @note Not used yet, but may be in the future on platforms with
 * remappable memory pages.
 */
#define kmalloc_buf_lock() do { ; } while (0)

/**
 * @brief Unlocks access to the kernel buffer cache.
 */
#define kmalloc_buf_unlock() do { ; } while (0)

/**
 * @brief Allocates data in process space to load and relocate a process.
 *
 * @param[in] size Number of bytes of process space data to allocate.
 *
 * @return A pointer into process space, or NULL if there is insufficient
 * memory available.
 */
void *kmalloc_proc_alloc(size_t size);

#ifdef __cplusplus
}
#endif

#endif