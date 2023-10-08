/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#ifndef MOSNIX_FS_FATFS_H
#define MOSNIX_FS_FATFS_H

#include <mosnix/inode.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Extra information about a FAT filesystem inode.
 */
struct fatfs_inode_info
{
    /** First cluster associated with the file or directory */
    uint32_t first_cluster;

    /** Size of the file in bytes */
    uint32_t size;

    /** Current cluster for the seek position */
    uint32_t cluster;

    /** Block number within the current cluster */
    uint32_t block;

    /** Offset within the current block */
    uint16_t offset : 9;

    /** Number of blocks left until the end of the cluster */
    uint16_t blocks_left : 7;
};

/* Check the size of the "fatfs_inode_info" structure */
kmalloc_buf_size_check(fatfs_inode_info);

/**
 * @brief Initialize the FAT filesystem module.
 */
void fatfs_init(void);

/**
 * @brief Determine if this target has an SD card interface.
 *
 * @return Non-zero if an SD card interface is available, zero if not.
 */
int fatfs_have_sd(void);

/**
 * @brief Mount the SD card.
 *
 * @param[in] dir Directory within the root filesystem to mount on.
 * This is assumed to be a RAM filesystem node.
 *
 * @return Zero on success, or a negative error code.
 */
int fatfs_mount_sd(struct inode *dir);

#ifdef __cplusplus
}
#endif

#endif
