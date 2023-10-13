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
#include <mosnix/util.h>
#include <sys/queue.h>
#include <string.h>

/* Start and end of user space RAM from the linker script */
extern void *user_space_ram_start;
extern void *user_space_ram_end;

/**
 * @brief Generic structure of a buffer cache entry on the free list.
 */
struct kmalloc_buffer
{
    /** Next item in the free list */
    SLIST_ENTRY(kmalloc_buffer) next;

    /** Padding to the full buffer size */
    unsigned char padding[KMALLOC_BUF_SIZE - sizeof(struct kmalloc_buffer *)];
};
kmalloc_buf_size_check(kmalloc_buffer);

/**
 * @brief Block within the user space memory allocation region.
 */
struct kmalloc_user_block
{
    /** Link to the next block */
    SLIST_ENTRY(kmalloc_user_block) next;

    /** Size of this block in bytes */
    size_t size;
};

/* Storage for the buffer cache in the ".noinit" section */
static struct kmalloc_buffer buffers[CONFIG_NUM_BUFFERS] ATTR_SECTION_NOINIT;

/* Free buffer list */
SLIST_HEAD(kmalloc_buffer_list, kmalloc_buffer);
static struct kmalloc_buffer_list free_buffers;

/* User space free block list */
SLIST_HEAD(kmalloc_user_list, kmalloc_user_block);
static struct kmalloc_user_list free_blocks;

/* Minimum user block size, below which it is not worth splitting the block */
#define KMALLOC_MIN_BLOCK_SIZE 32

void kmalloc_init(void)
{
    unsigned index;
    struct kmalloc_user_block *block;

    /* Insert all buffers in the buffer cache into the free list */
    SLIST_INIT(&free_buffers);
    for (index = 0; index < CONFIG_NUM_BUFFERS; ++index) {
        SLIST_INSERT_HEAD(&free_buffers, &(buffers[index]), next);
    }

    /* Initially all of user space is one single unallocated block */
    block = (struct kmalloc_user_block *)&user_space_ram_start;
    block->size = ((char *)&user_space_ram_end) -
                  ((char *)&user_space_ram_start) -
                  sizeof(struct kmalloc_user_block);
    SLIST_INIT(&free_blocks);
    SLIST_INSERT_HEAD(&free_blocks, block, next);
}

ATTR_NOINLINE void *kmalloc_buf_alloc(void)
{
    struct kmalloc_buffer *buf = SLIST_FIRST(&free_buffers);
    if (buf) {
        SLIST_REMOVE_HEAD(&free_buffers, next);
        memset(buf, 0, sizeof(struct kmalloc_buffer));
        return (void *)buf;
    }
    return 0;
}

ATTR_NOINLINE void kmalloc_buf_free(void *buf)
{
    if (buf) {
        struct kmalloc_buffer *buf2 = (struct kmalloc_buffer *)buf;
        SLIST_INSERT_HEAD(&free_buffers, buf2, next);
    }
}

/* The user space memory allocator uses a first-fit strategy */

/* This function is missing from <sys/queue.h> but is more efficient than
 * the standard function SLIST_REMOVE() if we know the previous element. */
#define SLIST_REMOVE_AFTER(head, prevelm, elm, field) \
    do { \
        if ((prevelm)) { \
            (prevelm)->field.sle_next = (elm)->field.sle_next; \
        } else { \
            SLIST_REMOVE_HEAD((head), field); \
        } \
    } while (0)

ATTR_NOINLINE void *kmalloc_user_alloc(size_t size)
{
    struct kmalloc_user_block *block;
    struct kmalloc_user_block *block2;
    struct kmalloc_user_block *prev;

    /*
     * Round the size up to a multiple of 4 so that allocations are
     * aligned for use with the ".o65" executable file format, which
     * supports 1, 2, and 4 byte alignment.
     *
     * Note: ".o65" does also allow for 256-byte page alignment but
     * we are not supporting such files at present due to memory wastage.
     */
    size = (size + 3U) & ~((size_t)3);

    /* Search for the first free block that will fit our allocation */
    prev = NULL;
    SLIST_FOREACH(block, &free_blocks, next) {
        if (block->size >= size) {
            /* Split the block if the remaining space is significant enough */
            if ((size - block->size) >= KMALLOC_MIN_BLOCK_SIZE) {
                block2 = (struct kmalloc_user_block *)
                    ((char *)(block + 1)) + size;
                block2->size =
                    block->size - size - sizeof(struct kmalloc_user_block);
                SLIST_INSERT_AFTER(block, block2, next);
                block->size = size;
            }

            /* Remove the block from the free list */
            SLIST_REMOVE_AFTER(&free_blocks, prev, block, next);

            /* Return the allocated pointer to the caller */
            return (void *)(block + 1);
        }
        prev = block;
    }
    return 0;
}

ATTR_NOINLINE void kmalloc_user_free(void *ptr)
{
    struct kmalloc_user_block *block;
    struct kmalloc_user_block *current;
    struct kmalloc_user_block *prev;
    char *temp;

    /* Bail out if the pointer is NULL */
    if (!ptr)
        return;

    /* Find the block header */
    block = ((struct kmalloc_user_block *)ptr) - 1;

    /* Search through the free block list to find where to put it back */
    prev = NULL;
    SLIST_FOREACH(current, &free_blocks, next) {
        if (current > block) {
            /* Free block needs to be inserted prior to the current item.
             * First see if we can combine it with the current item. */
            temp = ((char *)(block + 1)) + block->size;
            if (((void *)temp) == ((void *)current)) {
                SLIST_REMOVE_AFTER(&free_blocks, prev, current, next);
                block->size += sizeof(struct kmalloc_user_block);
                block->size += current->size;
            }
            break;
        }
        prev = current;
    }

    /* Insert the block into the free list */
    if (prev) {
        /* See if we can combine the block with the previous one */
        temp = ((char *)(prev + 1)) + prev->size;
        if (((void *)temp) == ((void *)block)) {
            prev->size += sizeof(struct kmalloc_user_block);
            prev->size += block->size;
        } else {
            SLIST_INSERT_AFTER(prev, block, next);
        }
    } else {
        /* The free block is now the new head of the list */
        SLIST_INSERT_HEAD(&free_blocks, block, next);
    }
}

ATTR_NOINLINE char **kmalloc_copy_argv(int argc, char **argv)
{
    char **argv_copy;
    char *buf;
    const char *arg;
    size_t size;
    int index;

    /* Limit the argument count to avoid requesting too much memory */
    if (argc > CONFIG_ARGC_MAX) {
        argc = CONFIG_ARGC_MAX;
    }

    /* Determine how many bytes we will need */
    size = (argc + 1) * sizeof(char *);
    for (index = 0; index < argc; ++index) {
        arg = argv[index];
        if (arg)
            size += strlen_constrained(arg, CONFIG_ARG_MAX);
        ++size;
    }

    /* Allocate the memory for the argument block */
    buf = kmalloc_user_alloc(size);
    if (!buf) {
        return NULL;
    }
    argv_copy = (char **)buf;
    buf += (argc + 1) * sizeof(char *);

    /* Format the argument block */
    for (index = 0; index < argc; ++index) {
        arg = argv[index];
        if (arg) {
            size = strlen_constrained(arg, CONFIG_ARG_MAX);
            memcpy(buf, arg, size);
            buf += size;
            *buf++ = '\0';
        } else {
            /* Convert NULL arguments into empty strings for safety */
            *buf++ = '\0';
        }
    }
    argv_copy[argc] = NULL;
    return argv_copy;
}
