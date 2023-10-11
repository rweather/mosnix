/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <mosnix/kmalloc.h>
#include <mosnix/attributes.h>
#include <mosnix/config.h>
#include <stdint.h>
#include <string.h>

/* Start and end of user space process RAM from the linker script */
extern void *user_space_ram_start;
extern void *user_space_ram_end;

/**
 * @brief Generic structure of a buffer cache entry on the free list.
 */
struct kmalloc_buffer
{
    /** Next item in the free list */
    struct kmalloc_buffer *next;

    /** Padding to the full buffer size */
    unsigned char padding[KMALLOC_BUF_SIZE - sizeof(void *)];
};
kmalloc_buf_size_check(kmalloc_buffer);

/**
 * @brief Storage for the buffer cache in the ".noinit" section.
 */
static struct kmalloc_buffer buffers[CONFIG_NUM_BUFFERS] ATTR_SECTION_NOINIT;

/** Pointer to the head of the free buffer list */
static struct kmalloc_buffer *free_buffers;

void kmalloc_init(void)
{
    /* Mark all buffers in the buffer cache as free */
    unsigned index;
    buffers[0].next = 0;
    for (index = 1; index < CONFIG_NUM_BUFFERS; ++index) {
        buffers[index].next = &(buffers[index - 1]);
    }
    free_buffers = &(buffers[CONFIG_NUM_BUFFERS - 1]);
}

ATTR_NOINLINE void *kmalloc_buf_alloc(void)
{
    struct kmalloc_buffer *buf = free_buffers;
    if (buf) {
        free_buffers = buf->next;
        memset(buf, 0, sizeof(struct kmalloc_buffer));
        return (void *)buf;
    }
    return 0;
}

ATTR_NOINLINE void kmalloc_buf_free(void *buf)
{
    if (buf && buf >= ((void *)buffers) &&
            buf <= ((void *)&buffers[CONFIG_NUM_BUFFERS - 1])) {
        struct kmalloc_buffer *buf2 = (struct kmalloc_buffer *)buf;
        buf2->next = free_buffers;
        free_buffers = buf2;
    }
}

ATTR_NOINLINE void *kmalloc_proc_alloc(size_t size)
{
    /* TODO */
    (void)size;
    return 0;
}
