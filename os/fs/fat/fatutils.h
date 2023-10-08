/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#ifndef MOSNIX_FS_FATUTILS_H
#define MOSNIX_FS_FATUTILS_H

#include "fatdefs.h"
#include <dirent.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Formats a name in 8.3 format.
 *
 * @param[out] out Returns the formatted name.
 * @param[in] name Points to the name for format.
 * @param[in] namelen Length of the name to be formatted.
 *
 * @return Non-zero on success, or zero if there is something wrong
 * with the @a name.
 */
uint8_t fat_format_name
    (char out[FAT_NAME_LEN], const char *name, size_t namelen);

/**
 * @brief Converts a FAT directory entry into a "dirent" structure.
 *
 * @param[out] out The output "dirent" structure.
 * @param[in] entry The input FAT directory entry.
 *
 * @return Non-zero if the entry was converted, or zero if the directory
 * entry cannot be converted because it is hidden or special.
 */
uint8_t fat_convert_dirent
    (struct dirent *out, const struct fat_dir_entry *entry);

/**
 * @brief Gets the cluster value in a FAT entry.
 *
 * @param[in] cluster The cluster number to fetch.
 * @param[out] value Returns the value in the FAT for @a cluster.
 *
 * @return Non-zero on success, or zero if something is wrong.
 */
uint8_t fat_get(uint32_t cluster, uint32_t *value);

/**
 * @brief Gets the next cluster in a FAT chain.
 *
 * @param[in] cluster The current cluster number.
 *
 * @return The new cluster number or 0 for the end of the chain.
 */
uint32_t fat_get_next_cluster(uint32_t cluster);

/**
 * @brief Converts the date and time in a FAT directory entry into a Unix time.
 *
 * @param[in] entry Points to the FAT directory entry.
 *
 * @return The Unix time value.
 */
time_t fat_convert_time_to_unix(const struct fat_dir_entry *entry);

#ifdef __cplusplus
}
#endif

#endif
