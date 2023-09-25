/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#ifndef MOSNIX_INODE_H
#define MOSNIX_INODE_H

#include <mosnix/kmalloc.h>
#include <mosnix/config.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Extra open mode that indicates "executable" or "searchable directory" */
#define O_EXEC 3

struct inode_operations;
struct file;

/**
 * @brief Structure of an inode in a filesystem.  This is the in-memory
 * representation of the inode data while it is being operated on.
 *
 * The inodes are allocated from the buffer cache, so there can be a
 * large number of them active at once.
 *
 * @note The buffer cache lock must be held while operating on inodes.
 */
struct inode
{
    /** Operations on this inode */
    const struct inode_operations *op;

    /** Number of references to the inode */
    short count;

    /** Type and permission bits */
    mode_t mode;

#if CONFIG_ACCESS_UID
    /** User that owns this inode */
    uid_t uid;

    /** Group that owns this inode */
    gid_t gid;
#endif

    /** Last modification time */
    time_t mtime;

    union {
        /** Filesystem-specific information about the inode's location */
        unsigned long info;

        /** Information specific to the RAM filesytem */
        struct {
            /** Points to the data for the file or symbolic link */
            struct ramfs_data *data;

            /** Size of the file or symbolic link data in bytes */
            u_short size;
        } ramfs_file;

        /** Pointer to the first entry in a RAM filesystem directory */
        struct ramfs_dirent *ramfs_dir;

        /** Device number if this inode is a character or block device */
        dev_t device;
    };
};

/**
 * @brief Operations that can be performed on an inode.
 *
 * Each filesystem defines its own version of this structure with the
 * allowed operations.
 *
 * The "release", "lookup", and "open" members are mandatory.  Other fields
 * can be NULL if the feature does not exist for the inode type.
 */
struct inode_operations
{
    /**
     * @brief Releases an inode which has reached a reference count of zero.
     *
     * @param[in] inode The inode to be released.
     *
     * @return Zero on success or a negative error code.  Even on error,
     * the inode will still be released.
     */
    int (*release)(struct inode *inode);

    /**
     * @brief Looks up the inode for a child of a directory inode by name.
     *
     * @param[out] inode Returns the child inode on success, with an
     * extra reference.
     * @param[in] dir The directory to look in.
     * @param[in] name Points to the name to look for.
     * @param[in] namelen Length of the name to look for.
     *
     * @return Zero on success, or an error code.
     */
    int (*lookup)(struct inode **inode, struct inode *dir,
                  const char *name, size_t namelen);

    /**
     * @brief Opens a file using the underlying filesystem.
     *
     * @param[in,out] file The file structure.
     *
     * @return Zero on success, or a negative error code otherwise.
     *
     * The "inode" member of @a file will already be populated if it is an
     * inode-based file.
     *
     * The "op" member of @a file must be set by this function on success.
     */
    int (*open)(struct file *file);

    /**
     * @brief Creates a new inode as a child of a directory inode.
     *
     * @param[out] inode Returns the child inode on success, with an
     * extra reference.
     * @param[in] dir The directory to look in.
     * @param[in] name Points to the name to look for.
     * @param[in] namelen Length of the name to look for.
     * @param[in] mode The mode for the new node.
     *
     * @return Zero on success, or an error code.
     *
     * If @a mode indicates a directory, then this function must create
     * the "." and ".." links for the new directory.
     *
     * This function may be NULL if the underlying filesystem is read-only.
     */
    int (*mknod)(struct inode **inode, struct inode *dir,
                 const char *name, size_t namelen, mode_t mode);

#if CONFIG_SYMLINK
    /**
     * @brief Reads the contents of a symbolic link inode.
     *
     * @param[in] inode The inode to read.
     * @param[in] buf The buffer to read the link's contents into.
     * @param[in] size Size of the buffer.
     *
     * The number of characters in the link's contents that were
     * written to @a buf, or a negative error code.
     *
     * This function may be NULL if the underlying filesystem does not
     * support symbolic links.
     */
    ssize_t (*readlink)(struct inode *inode, char *buf, size_t size);
#endif
};

/* Check that the inode structure fits within a buffer cache entry */
kmalloc_buf_size_check(inode);

/**
 * @brief Allocates memory from the buffer cache for an inode.
 *
 * @param[in] operations Points to the operations table for the inode type.
 *
 * @return A pointer to the new inode, or NULL if out of buffer cache memory.
 *
 * The reference count of the new inode will be 1.
 */
struct inode *inode_alloc(const struct inode_operations *operations);

/**
 * @brief Adds another reference to an inode.
 *
 * @param[in,out] inode The inode to add the reference to.
 */
static inline void inode_ref(struct inode *inode)
{
    ++(inode->count);
}

/**
 * @brief Removes a reference from an inode and frees it if there are
 * no more references left.
 *
 * @param[in] inode The inode to dereference.
 *
 * @return Zero on success or an error code otherwise.
 */
int inode_deref(struct inode *inode);

/**
 * @brief Gets a reference to the root of the filesystem tree.
 *
 * @return The root inode.
 *
 * This function must be called with the buffer cache lock held.
 */
struct inode *inode_get_root(void);

/**
 * @brief Resolves a pathname to absolute form.
 *
 * @param[out] out Output absolute path.
 * @param[in] outlen Length of the @a out buffer.
 * @param[in] pathname The pathname, relative or absolute, to resolve.
 *
 * @return 0 if the pathname was resolved, 1 if the pathname was resolved
 * but the last character was a '/', or a negative error code otherwise.
 *
 * If @a pathname is relative, it will be resolved relative to the
 * working directory of the current process.
 *
 * This function does not touch the filesystem.  It resolves relative
 * path references as well as "." and ".." components without touching
 * the actual filesystem.  This does permit technically invalid
 * paths like "/a/nonexistent/../b" to resolve to "/a/b" even though the
 * "nonexistent" name is not present in the filesystem.
 *
 * While this is not technically POSIX, it makes the implementation
 * simpler to separate pathname resolution from walking the filesystem.
 *
 * The buffer cache lock must not be held when calling this function
 * if @a pathname points into user space.
 */
int inode_path_to_abs(char *out, size_t outlen, const char *pathname);

/**
 * @brief Resolves a symbolic link relative to an existing absolute pathname.
 *
 * @param[in,out] out Absolute path on input and output.
 * @param[in] outlen Length of the @a out buffer.
 * @param[in] pathname The pathname, relative or absolute, to resolve.
 *
 * @return 0 if the pathname was resolved, 1 if the pathname was resolved
 * but the last character was a '/', or a negative error code otherwise.
 *
 * The buffer cache lock must not be held when calling this function
 * if @a pathname points into user space.
 */
int inode_symlink_to_abs(char *out, size_t outlen, const char *pathname);

/**
 * @brief Lookup an absolute pathname in the filesystem.
 *
 * @param[out] inode The inode for the path if it was found.
 * @param[in] pathname The absolute pathname to look for, which must be
 * the output of inode_path_to_abs() or inode_symlink_to_abs().  The first
 * character of the @a pathname is assumed to be '/'.
 * @param[in] oflag The open flags, including creation flags.
 * @param[in] mode The mode to create a new inode with, or the type of
 * node that we expect to find such as S_IFREG.
 * @param[in] follow Non-zero to follow the last symbolic link in a
 * path, or zero to stop at the last item if it is a symbolic link.
 *
 * @return Zero on success with a pre-existing inode, 1 on success
 * if a new inode was created, or a negative error code.
 *
 * This function must be called with the buffer cache lock held.
 * The returned @a inode will have its reference count incremented by 1.
 *
 * The contents of the @a pathname argument may be destroyed if the
 * lookup involves following symbolic links.  It must point to a buffer
 * of at least CONFIG_PATH_MAX characters in length.
 */
int inode_lookup(struct inode **inode, char *pathname, int oflag,
                 mode_t mode, u_char follow);

/**
 * @brief Lookup a pathname in the filesystem.
 *
 * @param[out] inode The inode for the path if it was found.
 * @param[in] pathname The pathname to look for, which may be absolute or
 * relative.
 * @param[in] oflag The open flags, including creation flags.
 * @param[in] mode The mode to create a new inode with, or the type of
 * node that we expect to find such as S_IFREG.
 * @param[in] follow Non-zero to follow the last symbolic link in a
 * path, or zero to stop at the last item if it is a symbolic link.
 *
 * @return Zero on success with a pre-existing inode, 1 on success
 * if a new inode was created, or a negative error code.
 *
 * On success, the buffer cache lock will be held and the returned
 * @a inode will have its reference count incremented by 1.
 */
int inode_lookup_path(struct inode **inode, const char *pathname, int oflag,
                      mode_t mode, u_char follow);

/**
 * @brief Determine if the user has the requested permission on an inode.
 *
 * @param[in] inode The inode to check.
 * @param[in] mode The mode; F_OK, R_OK, W_OK, or X_OK.
 *
 * @return Zero if access is allowed, or -EACCES if not allowed.
 */
int inode_access(const struct inode *inode, int mode);

/**
 * @brief Gets a value suitable for use in the mtime field of an inode.
 *
 * @return The time value.
 */
time_t inode_get_mtime(void);

#ifdef __cplusplus
}
#endif

#endif
