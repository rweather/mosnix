/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#ifndef MOSNIX_FS_FATDEFS_H
#define MOSNIX_FS_FATDEFS_H

#include "drivers/sdcard/sdcard.h"

#ifdef __cplusplus
extern "C" {
#endif

/** End of directory / free entry */
#define FAT_DIR_END             ((char)0x00)

/** Deleted entry */
#define FAT_DIR_DELETED         ((char)0xE5)

/** File is read-only */
#define FAT_ATTR_READ_ONLY      ((uint8_t)0x01)

/** File is hidden */
#define FAT_ATTR_HIDDEN         ((uint8_t)0x02)

/** File is system-related */
#define FAT_ATTR_SYSTEM         ((uint8_t)0x04)

/** Volume label */
#define FAT_ATTR_VOLUME         ((uint8_t)0x08)

/** File is a directory */
#define FAT_ATTR_DIRECTORY      ((uint8_t)0x10)

/** File archive bit */
#define FAT_ATTR_ARCHIVE        ((uint8_t)0x20)

/** File is a device */
#define FAT_ATTR_DEVICE         ((uint8_t)0x40)

/** Reserved file attribute */
#define FAT_ATTR_RESERVED       ((uint8_t)0x80)

/** Long filename declarations have these attributes */
#define FAT_ATTR_LFN            ((uint8_t)0x0F)

/**
 * @brief Directory entries with these attributes are blocked from being
 * listed in a mounted FAT filesystem.
 */
#define FAT_ATTR_BLOCKED \
    (FAT_ATTR_HIDDEN | FAT_ATTR_SYSTEM | FAT_ATTR_VOLUME | \
     FAT_ATTR_DEVICE | FAT_ATTR_RESERVED)

/** No case information in the "filename_case" field */
#define FAT_CASE_NONE           ((uint8_t)0x00)

/** Lowercase the extension */
#define FAT_CASE_LOWER_EXT      ((uint8_t)0x08)

/** Lowercase the basename */
#define FAT_CASE_LOWER_BASE     ((uint8_t)0x10)

/** Length of a formatted 8.3 filename */
#define FAT_NAME_LEN            11

/**
 * @brief Special cluster number at the end of a FAT chain.
 *
 * Any value greater than or equal to this indicates the end of a chain.
 */
#define FAT_END_CLUSTER 0x0FFFFFF8UL

/**
 * @brief Number of directory entries per 512-byte block.
 */
#define FAT_DIR_ENTRIES_PER_BLOCK 16

/**
 * @brief Directory entry in a FAT filesystem.
 *
 * https://en.wikipedia.org/wiki/Design_of_the_FAT_file_system#Directory_entry
 */
struct fat_dir_entry
{
    /** Name of the directory entry; 8.3 padded with spaces.
     *
     * The first character may be FAT_DIR_END or FAT_DIR_DELETED
     * if the entry is special. */
    char name[FAT_NAME_LEN];

    /** File attributes */
    uint8_t attrs;

    /** Some combination of FAT_CASE_LOWER_EXT and FAT_CASE_LOWER_BASE */
    uint8_t filename_case;

    /** Fractional creation time, 0..199 deciseconds */
    uint8_t ctime_ds;

    /** Creation time in 2 second units */
    uint16_t ctime;

    /** Creation date */
    uint16_t cdate;

    /** Last access date */
    uint16_t adate;

    /** High word of the cluster number for the file or directory */
    uint16_t cluster_high;

    /** Last modification time */
    uint16_t mtime;

    /** Last modification date */
    uint16_t mdate;

    /** Low word of the cluster number for the file or directory */
    uint16_t cluster_low;

    /** Size of the file in bytes */
    uint32_t size;
};

#ifdef __cplusplus
}
#endif

#endif
