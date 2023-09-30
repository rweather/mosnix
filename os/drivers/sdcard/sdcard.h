/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#ifndef MOSNIX_DRIVERS_SDCARD_H
#define MOSNIX_DRIVERS_SDCARD_H

#include "drivers/spi/spi.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief No SD card is present in the slot.
 */
#define SD_DETECT_NONE      0

/**
 * @brief A new SD card is present in the slot that needs initial
 * configuration to be used.
 */
#define SD_DETECT_NEW       1

/**
 * @brief The existing SD card from the last detection is still present.
 */
#define SD_DETECT_EXISTING  2

/**
 * @brief Size of a single block on an SD card.
 */
#define SD_BLKSIZE  512

/**
 * @brief Information about the inserted SD card and FAT32 volume.
 */
typedef struct
{
    /** Detect flag, indicating if there is an SD card and its version.
     *  Zero if the SD card is not currently mounted. */
    uint8_t detect;

    /** Non-zero if the SD card supports SDHC */
    uint8_t sdhc;

    /** Cluster size in blocks: 1, 2, 4, 8, ..., 128 (must be a power of 2) */
    uint8_t cluster_size;

    /** Number of bits to shift that corresponds to "cluster_size" */
    uint8_t cluster_shift;

    /** Offset of the first partition on the SD card (or the whole card) */
    uint32_t part_offset;

    /** Number of 512-byte blocks in the partition */
    uint32_t part_size;

    /** Starting block number for the first FAT */
    uint32_t fat1;

    /** Starting block number for the second FAT */
    uint32_t fat2;

    /** Size of each FAT in blocks */
    uint32_t fat_size;

    /** Cluster number of the root directory */
    uint32_t root;

    /** Block number of the start of the data clusters */
    uint32_t cluster_offset;

    /** Number of clusters in the partition */
    uint32_t cluster_count;

    /** Cached block */
    uint8_t block[SD_BLKSIZE];

    /** Sector number of the cached block */
    uint32_t cache_block;

    /** Sector number of the second copy for FAT write operations */
    uint32_t cache_block2;

    /** Mode for the cached block (0 if nothing cached) */
    uint8_t cache_mode;

} sd_info_t;

/**
 * @brief Information about the mounted SD card and FAT32 partition.
 */
extern sd_info_t sd_info;

/**
 * @brief Initializes the port pins for the SD card interface.
 * Does not communicate with the card yet.
 */
void sd_init(void);

/**
 * @brief Detects the presence of an SD card in the slot.
 *
 * @return SD_DETECT_NONE, SD_DETECT_NEW, or SD_DETECT_EXISTING.
 */
uint8_t sd_detect(void);

/**
 * @brief Re-detects the presence of an SD card in the slot, restarting
 * the detection from scratch.
 *
 * @return SD_DETECT_NONE, SD_DETECT_NEW, or SD_DETECT_EXISTING.
 */
uint8_t sd_redetect(void);

/**
 * @brief Converts a cluster number into a sector block number.
 *
 * @param[in] cluster The cluster number.
 *
 * @return The block number, or 0 if the cluster number is invalid.
 */
uint32_t sd_cluster_to_block(uint32_t cluster);

/**
 * @brief Reads a block of data from the SD card.
 *
 * @param[in] blknum Block number to read from.
 *
 * @return The response byte for the command, which will be SD_R1_READY if
 * the block was read successfully.
 */
uint8_t sd_read(uint32_t blknum);

/**
 * @brief Writes a block of data to the SD card.
 *
 * @param[in] blknum Block number to write to.
 *
 * @return The response byte for the command.
 */
uint8_t sd_write(uint32_t blknum);

/**
 * @brief Reads a block from the cache, or the physical SD media.
 *
 * @param[in] blknum Block number to read from.
 *
 * @return The response byte for the command, which will be SD_R1_READY if
 * the block was read successfully.
 *
 * If the block is already in the cache, then this will return immediately
 * without reading it again.
 */
uint8_t sd_cache_read(uint32_t blknum);

/**
 * @brief Prepares to write a block to SD media.
 *
 * @param[in] blknum Block number to write to.
 *
 * @return The response byte for the command.
 *
 * This function will read the current contents of the block into the cache
 * and prepare it for writing.
 */
uint8_t sd_cache_write_prepare(uint32_t blknum);

/**
 * @brief Prepares to write a FAT block to SD media.
 *
 * @param[in] blknum FAT block number to write to.
 *
 * @return The response byte for the command.
 *
 * This block number is assumed to be within FAT1.  This function will also
 * prepare to write a copy of the data to FAT2 when the next flush happens.
 */
uint8_t sd_cache_write_prepare_fat(uint32_t blknum);

/**
 * @brief Prepares to write a block to SD media that is initially zero.
 *
 * @param[in] blknum Block number to write to.
 *
 * @return The response byte for the command.
 *
 * This function will fill the cache block with zeroes in the cache and
 * prepare it for writing.  This is faster than sd_cache_write_prepare()
 * when a new block is being written to a file.
 */
uint8_t sd_cache_write_prepare_zero(uint32_t blknum);

/**
 * @brief Flushes pending writes to the SD card.
 *
 * @return The response byte for the command.
 */
uint8_t sd_cache_flush(void);

#ifdef __cplusplus
}
#endif

#endif
