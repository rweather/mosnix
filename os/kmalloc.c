/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <mosnix/kmalloc.h>
#include <stdint.h>

/* Start and end of user space process RAM from the linker script */
extern void *user_space_ram_start;
extern void *user_space_ram_end;

/* Start and end of buffer cache RAM from the linker script */
extern void *buffer_cache_ram_start;
extern void *buffer_cache_ram_end;

/** Pointer to the head of the free buffer list */
static void *free_buffers;

void kmalloc_init(void)
{
    /* Find all all buffers in the buffer cache and mark them as free */
    uintptr_t size = ((uintptr_t)&buffer_cache_ram_end) -
                     ((uintptr_t)&buffer_cache_ram_start);
    char *ptr = (char *)&buffer_cache_ram_start;
    free_buffers = 0;
    while (size >= KMALLOC_BUF_SIZE) {
        *((void **)ptr) = free_buffers;
        free_buffers = (void *)ptr;
        ptr += KMALLOC_BUF_SIZE;
        size -= KMALLOC_BUF_SIZE;
    }
}

void *kmalloc_buf_alloc(void)
{
    void *buf = free_buffers;
    if (buf) {
        free_buffers = *((void **)buf);
        return buf;
    }
    return 0;
}

void kmalloc_buf_free(void *buf)
{
    if (buf && buf >= ((void *)&buffer_cache_ram_start) &&
            buf < ((void *)&buffer_cache_ram_end)) {
        *((void **)buf) = free_buffers;
        free_buffers = buf;
    }
}

void *kmalloc_proc_alloc(size_t size)
{
    /* TODO */
    (void)size;
    return 0;
}
