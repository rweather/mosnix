/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include "fatutils.h"
#include <mosnix/attributes.h>
#include <sys/stat.h>
#include <string.h>

ATTR_NOINLINE uint8_t fat_format_name
    (char out[FAT_NAME_LEN], const char *name, size_t namelen)
{
    static char const invalid_chars[] = "\"*+,/:;<=>?[\\]^|";
    uint8_t posn;
    uint8_t dot;

    /* Cannot format if the name is empty */
    if (namelen == 0) {
        return 0;
    }

    /* Pad the output with spaces before we start */
    memset(out, ' ', FAT_NAME_LEN);

    /* Check for "." and ".." */
    if (name[0] == '.') {
        if (namelen == 1) {
            out[0] = '.';
            return 1;
        } else if (namelen == 2 && name[1] == '.') {
            out[0] = '.';
            out[1] = '.';
            return 1;
        } else {
            /* '.' cannot start a filename on its own, except "." and ".." */
            return 0;
        }
    }

    /* Convert the name */
    posn = 0;
    dot = 0;
    while (namelen > 0) {
        char ch = *name++;
        if (ch < 0x21 || ch > 0x7E) {
            /* Only printable ASCII is allowed */
            return 0;
        } else if (ch == '.') {
            /* Switch from the basename to the extension */
            if (dot)
                return 0;
            posn = 8;
            dot = 1;
        } else if (strchr(invalid_chars, ch) != NULL) {
            /* This character is invalid in a 8.3 filename */
            return 0;
        }
        if (ch >= 'a' && ch <= 'z') {
            /* Convert lower case to upper case */
            ch ^= 0x20;
        }
        if (dot) {
            if (posn >= 11) {
                return 0;
            }
        } else {
            if (posn >= 8) {
                return 0;
            }
        }
        out[posn++] = ch;
        --namelen;
    }
    return 1;
}

static uint8_t fat_convert_name
    (char *outname, uint8_t posn, const char *inname, uint8_t len)
{
    while (len > 0) {
        char ch = *inname++;
        if (ch == ' ')
            break;
        else if (ch >= 'A' && ch <= 'Z')
            outname[posn] = ch ^ 0x20;
        else
            outname[posn] = ch;
        ++posn;
        --len;
    }
    return posn;
}

ATTR_NOINLINE uint8_t fat_convert_dirent
    (struct dirent *out, const struct fat_dir_entry *entry)
{
    const char *name;
    char ch;

    /* Bail out if an end of directory marker or if the entry is deleted */
    name = entry->name;
    ch = name[0];
    if (ch == FAT_DIR_END || ch == FAT_DIR_DELETED) {
        return 0;
    }

    /* Reject everything except regular files and directories.
     * This will also block long filename declarations. */
    if (entry->attrs & FAT_ATTR_BLOCKED) {
        return 0;
    }

    /* Convert the name from 8.3 format into a regular filename */
    if (ch != '.') {
        /* Convert the basename and extension */
        uint8_t posn = fat_convert_name(out->d_name, 0, name, 8);
        if (name[8] != ' ') {
            out->d_name[posn++] = '.';
            posn = fat_convert_name(out->d_name, posn, name + 8, 3);
        }
        out->d_name[posn] = '\0';
    } else {
        /* This entry will be either "." or ".." */
        out->d_name[0] = '.';
        if (name[1] == '.') {
            out->d_name[1] = '.';
            out->d_name[2] = '\0';
        } else {
            out->d_name[1] = '\0';
        }
    }

    /* Set the other "dirent" fields */
    out->d_ino = entry->size;
    out->d_mtime_np = fat_convert_time_to_unix(entry);
    if (entry->attrs & FAT_ATTR_READ_ONLY) {
        out->d_mode_np = 0555;
    } else {
        out->d_mode_np = 0755;
    }
    if (entry->attrs & FAT_ATTR_DIRECTORY) {
        out->d_type = DT_DIR;
        out->d_mode_np |= S_IFDIR;
    } else {
        out->d_type = DT_REG;
        out->d_mode_np |= S_IFREG;
    }
    return 1;
}

ATTR_NOINLINE uint8_t fat_get(uint32_t cluster, uint32_t *value)
{
    unsigned offset;

    /* Bail out if the cluster number is out of range */
    if (cluster > (sd_info.cluster_count + 1)) {
        return 0;
    }

    /* Cache the relevant FAT block for reading */
    if (sd_cache_read(sd_info.fat1 + (cluster >> 7)) != 0) {
        return 0;
    }

    /* Extract the cluster number in the FAT entry */
    offset = cluster & 0x0000007F;
    *value = ((const uint32_t *)(sd_info.block))[offset] & 0x0FFFFFFFUL;
    return 1;
}

ATTR_NOINLINE uint32_t fat_get_next_cluster(uint32_t cluster)
{
    uint32_t next;
    if (fat_get(cluster, &next) && next < FAT_END_CLUSTER) {
        return next;
    } else {
        return 0;
    }
}

time_t fat_convert_time_to_unix(const struct fat_dir_entry *entry)
{
    /* TODO */
    (void)entry;
    return 0;
}
