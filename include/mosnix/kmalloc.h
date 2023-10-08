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
 * The return buffer will be initialized to all-zeroes.
 */
void *kmalloc_buf_alloc(void);

/**
 * @brief Frees a buffer back to the kernel's buffer cache.
 *
 * @param[in] buf Points to the buffer to free.
 */
void kmalloc_buf_free(void *buf);

/**
 * @brief Compile-time check that a structure can fit in a buffer cache entry.
 *
 * @param[in] type The name of the type, without the "struct".
 *
 * This will give a compile-time error if "struct type" will not fit within
 * the bounds of a buffer cache entry.
 */
#define kmalloc_buf_size_check(type) \
    typedef char type##_buf_size_check \
        [(sizeof(struct type) <= KMALLOC_BUF_SIZE) * 2 - 1]

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
