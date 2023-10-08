/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include "fatfs.h"
#include "fatdefs.h"
#include "fatutils.h"
#include <mosnix/target.h>
#include <mosnix/file.h>
#include <mosnix/proc.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#if defined(CONFIG_SD) && defined(CONFIG_SPI)

#include "drivers/spi/spi.h"
#include "drivers/sdcard/sdcard.h"

/* Forward declaration */
extern struct inode_operations const fatfs_operations;

/**
 * @brief Initializes a fatfs_inode_info structure when opening a
 * file or directory.
 *
 * @param[out] info The information structure to initialize.
 * @param[in] cluster First cluster for the file or directory.
 * @param[in] size Size of the file, or 0 if this is a directory.
 *
 * @return Non-zero on success, or zero if an error occurred.
 */
static uint8_t fatfs_info_new
    (struct fatfs_inode_info *info, uint32_t cluster, uint32_t size)
{
    info->first_cluster = info->cluster = cluster;
    info->size = size;
    if ((info->block = sd_cluster_to_block(cluster)) == 0) {
        return 0;
    }
    info->offset = 0;
    info->blocks_left = sd_info.cluster_size;
    return 1;
}

/**
 * @brief Reads the next FAT directory entry.
 *
 * @param[in,out] info The information structure for the FAT directory.
 * @param[out] entry Returns a pointer to the directory entry on exit.
 *
 * @return 0 at EOF, 1 if an entry was read, or a negative error code.
 */
static int fatfs_info_read_dirent
    (struct fatfs_inode_info *info, const struct fat_dir_entry **entry)
{
    uint32_t block = info->block;
    uint16_t offset;

    /* Did we already reach EOF previously? */
    if (block == 0) {
        return 0;
    }

    /* Do we need to advance to the next block or cluster? */
    offset = info->offset;
    if (offset >= SD_BLKSIZE) {
        uint8_t blocks_left = info->blocks_left - 1;
        if (blocks_left == 0) {
            /* Advance to the next cluster */
            uint32_t cluster = fat_get_next_cluster(info->cluster);
            info->cluster = cluster;
            if (cluster == 0) {
                /* We are at the end of the cluster chain */
                info->block = 0;
                return 0;
            }
            block = sd_cluster_to_block(cluster);
            if (block == 0) {
                /* Could not resolve the next cluster to a block */
                return -EIO;
            }
            blocks_left = sd_info.cluster_size;
        } else {
            /* Advance to the next block in the current cluster */
            ++block;
        }
        info->block = block;
        info->blocks_left = blocks_left;
        offset = info->offset = 0;
    }

    /* Read the current block into the cache */
    if (sd_cache_read(block) != 0) {
        /* Failed to read the block.  Most likely cause is that the
         * SD card has an error or it has been removed. */
        return -EIO;
    }

    /* Return a pointer to the next directory entry and skip over it */
    *entry = (const struct fat_dir_entry *)(sd_info.block + offset);
    info->offset = offset + sizeof(struct fat_dir_entry);
    return 1;
}

static ssize_t fatfs_dir_read(struct file *file, void *data, size_t size)
{
    struct fatfs_inode_info *info = file->fatfs_info;
    struct dirent *out = (struct dirent *)data;
    const struct fat_dir_entry *entry;
    ssize_t result = 0;
    int error;
    while (size >= sizeof(struct dirent)) {
        error = fatfs_info_read_dirent(info, &entry);
        if (error < 0) {
            return error;
        } else if (!error) {
            break;
        }
        if (entry->name[0] == FAT_DIR_END) {
            break;
        }
        if (fat_convert_dirent(out, entry)) {
            result += sizeof(struct dirent);
            size -= sizeof(struct dirent);
            ++out;
        }
        file->posn += sizeof(struct dirent);
    }
    return result;
}

static int fatfs_close(struct file *file)
{
    kmalloc_buf_free(file->fatfs_info);
    return file_close_default(file);
}

static struct file_operations const fatfs_dir_operations = {
    .close = fatfs_close,
    .read = fatfs_dir_read,
    .write = file_write_default,
    .lseek = file_lseek_default,
    .ioctl = file_ioctl_default
};

static int fatfs_release(struct inode *inode)
{
    kmalloc_buf_free(inode->fatfs_info);
    return 0;
}

static struct inode *fatfs_create_inode
    (const struct fat_dir_entry *entry, struct inode *parent)
{
    struct fatfs_inode_info *info;
    struct inode *inode = inode_alloc(&fatfs_operations);
    if (!inode) {
        return 0;
    }
    info = kmalloc_buf_alloc();
    if (!info) {
        inode_deref(inode);
        return 0;
    }
    inode->fatfs_info = info;
#if CONFIG_ACCESS_UID
    /* Inherit ownership information from the parent directory */
    inode->uid = parent->uid;
    inode->gid = parent->gid;
#else
    (void)parent;
#endif
    inode->mtime = fat_convert_time_to_unix(entry);
    info->first_cluster =
        (((uint32_t)(entry->cluster_high)) << 16) | entry->cluster_low;
    info->size = entry->size;
    if (entry->attrs & FAT_ATTR_READ_ONLY) {
        inode->mode = 0555;
    } else {
        inode->mode = 0755;
    }
    if (entry->attrs & FAT_ATTR_DIRECTORY) {
        inode->mode |= S_IFDIR;
    } else {
        inode->mode |= S_IFREG;
    }
    return inode;
}

static int fatfs_lookup
    (struct inode **inode, struct inode *dir, const char *name, size_t namelen)
{
    char name83[FAT_NAME_LEN];
    struct fatfs_inode_info reader;
    uint32_t cluster;
    const struct fat_dir_entry *entry;
    uint16_t max_entries;
    int result;

    /* The inode must be a directory or this doesn't make sense */
    if (!S_ISDIR(dir->mode)) {
        return -ENOTDIR;
    }

    /* Convert the name component into 8.3 format */
    if (!fat_format_name(name83, name, namelen)) {
        return -EINVAL;
    }

    /* Find the cluster number for the directory.  If we are looking at
     * the root, then force SD card detection in case the card has been
     * removed since the last time we did a pathname lookup.  If the
     * card is still inserted, then detection is fairly quick. */
    cluster = dir->fatfs_info->first_cluster;
    if (cluster == FAT_END_CLUSTER) {
        if (sd_detect() == SD_DETECT_NONE) {
            /* SD card is not inserted, so all lookups will fail */
            return -ENOENT;
        }
        cluster = sd_info.root;
    }

    /* Search the directory for the name */
    max_entries = 65535;
    if (!fatfs_info_new(&reader, cluster, 0)) {
        return -EIO;
    }
    do {
        /* Read the next directory entry */
        result = fatfs_info_read_dirent(&reader, &entry);
        if (result < 0) {
            return result;
        } else if (!result) {
            break;
        }

        /* Is this the entry we were looking for? */
        if (entry->name[0] == FAT_DIR_END) {
            /* We have reached the end of the directory */
            break;
        } else if (entry->name[0] == FAT_DIR_DELETED) {
            /* This entry has been deleted */
            continue;
        } else if (entry->attrs & FAT_ATTR_BLOCKED) {
            /* Hidden or system file that should never match */
            continue;
        } else if (!memcmp(entry->name, name83, FAT_NAME_LEN)) {
            /* We have found the entry we were after */
            struct inode *new_inode = fatfs_create_inode(entry, dir);
            if (new_inode) {
                *inode = new_inode;
                return 0;
            } else {
                return -ENOMEM;
            }
        }
    } while ((--max_entries) > 0);

    /* The name does not exist in the directory */
    return -ENOENT;
}

static int fatfs_open(struct file *file)
{
    struct fatfs_inode_info *info = file->inode->fatfs_info;
    if (S_ISDIR(file->mode)) {
        /* Find the first cluster in the directory */
        uint32_t cluster = info->first_cluster;
        if (cluster == FAT_END_CLUSTER) {
            /* This is actually the root directory of the SD card.
             * Force a detection in case the SD card was removed since
             * the last time we tried to access the card. */
            if (sd_detect() == SD_DETECT_NONE) {
                /* SD card is no longer inserted */
                return -EIO;
            }
            cluster = sd_info.root;
        }

        /* Create a new information block for the file structure */
        info = kmalloc_buf_alloc();
        if (!info) {
            return -ENOMEM;
        }
        file->fatfs_info = info;
        if (!fatfs_info_new(info, cluster, 0)) {
            return -EIO;
        }

        /* Set the file descriptor operations table */
        file->op = &fatfs_dir_operations;
        return 0;
    } else {
        /* TODO: opening regular files */
        (void)file;
        return -EPERM;
    }
}

/**
 * @brief Operations for the FAT filesystem.
 */
struct inode_operations const fatfs_operations = {
    .release = fatfs_release,
    .lookup = fatfs_lookup,
    .open = fatfs_open
};

void fatfs_init(void)
{
    sd_init();
}

int fatfs_have_sd(void)
{
    return 1;
}

int fatfs_mount_sd(struct inode *dir)
{
    struct inode *root = 0;
    struct fatfs_inode_info *info;
    int result;

    /* Create an inode for the FAT root directory */
    root = inode_alloc(&fatfs_operations);
    if (!root) {
        return -ENOMEM;
    }
    info = kmalloc_buf_alloc();
    if (!info) {
        inode_deref(root);
        return -ENOMEM;
    }
    info->first_cluster = FAT_END_CLUSTER; /* Indicates the root directory */
    root->mode = S_IFDIR | 0755;
#if CONFIG_ACCESS_UID
    /* Set the root directory's ownership to that of the mounting process */
    root->uid = current_proc->euid;
    root->gid = current_proc->egid;
#endif
    root->mtime = inode_get_mtime();
    root->fatfs_info = info;

    /* Detect the presence of the SD card */
    result = sd_detect();

    /* Attach the FAT root directory to the mount point */
    dir->mode |= S_ISVTX;
    dir->ramfs_mount = root;

    /* Returns 1 if there is no SD card.  We still mount the filesystem
     * even if there is no SD card, and re-detect during lookup. */
    return (result == SD_DETECT_NONE) ? 1 : 0;
}

#else /* !(CONFIG_SD && CONFIG_SPI) */

/* Stub out the FAT filesystem operations on systems without an SD card */
void fatfs_init(void) {}
int fatfs_have_sd(void) { return 0; }
int fatfs_mount_sd(struct inode *dir) { (void)dir; return -ENODEV; }

#endif
