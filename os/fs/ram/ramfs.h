/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#ifndef MOSNIX_FS_RAMFS_H
#define MOSNIX_FS_RAMFS_H

#include <mosnix/inode.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Fragment of data for a file or symbolic link.
 */
struct ramfs_data
{
    /** Data within this fragment of the file or symbolic link */
    u_char data[16];

    /** Next data fragment within the file or symbolic link */
    struct ramfs_data *next;
};

/**
 * @brief Directory entry within a RAM filesystem.
 */
struct ramfs_dirent
{
    /** Length of the name in this directory entry */
    u_char namelen;

    /** Name of the directory entry, not NUL-terminated */
    char name[13];

    /** Points to the inode associated with the directory entry */
    struct inode *inode;

    /** Next entry in this directory */
    struct ramfs_dirent *next;
};

/* Check that all RAM filesystem structures fit within a buffer cache entry */
kmalloc_buf_size_check(ramfs_data);
kmalloc_buf_size_check(ramfs_dirent);

/**
 * @brief Operations for the RAM filesystem.
 */
extern struct inode_operations const ramfs_operations;

/**
 * @brief Initialize the RAM filesystem module.
 */
void ramfs_init(void);

#ifdef __cplusplus
}
#endif

#endif
