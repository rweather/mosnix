/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#ifndef MOSNIX_FILE_H
#define MOSNIX_FILE_H

#include <mosnix/config.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Standard seek modes */
#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif

/**
 * @typedef file_ref_count_t
 * @brief Reference count on a file object.
 *
 * Worst case is when the file descriptor is used in every fd slot of
 * every process, plus two extras per process for temporary references
 * in the system call layer.  Size the type appropriately.
 */
#if (CONFIG_PROC_MAX * (CONFIG_PROC_FD_MAX + 2)) < 127
typedef signed char file_ref_count_t;
#elif (CONFIG_PROC_MAX * (CONFIG_PROC_FD_MAX + 2)) < 32767
typedef short file_ref_count_t;
#else
typedef long file_ref_count_t;
#endif

struct file;
struct inode;

/**
 * @brief Table of operations on a file descriptor.
 *
 * This structure defines the type of file descriptor and the filesystem
 * or device category that it belongs to.
 *
 * All members must be supplied.  Nothing can be NULL.  There are default
 * functions like file_write_default(), file_ioctl_default(), etc that
 * can be used to error out unnecessary members.  This means that the
 * system call doesn't have to check if a member is NULL before calling it.
 */
struct file_operations
{
    /**
     * @brief Closes a file descriptor.
     *
     * @param[in] file The file descriptor to close.
     *
     * @return Zero on success, or an error code.
     */
    int (*close)(struct file *file);

    /**
     * @brief Reads data from a file descriptor.
     *
     * @param[in] file The file descriptor to read from.
     * @param[in] data Points to the buffer to read into.
     * @param[in] size The number of bytes to read.
     *
     * @return The number of bytes read, or a negative error code.
     */
    ssize_t (*read)(struct file *file, void *data, size_t size);

    /**
     * @brief Writes data to a file descriptor.
     *
     * @param[in] file The file descriptor to write to.
     * @param[in] data Points to the buffer to write from.
     * @param[in] size The number of bytes to write.
     *
     * @return The number of bytes written, or a negative error code.
     */
    ssize_t (*write)(struct file *file, const void *data, size_t len);

#if CONFIG_LSEEK
    /**
     * @brief Seeks within a file.
     *
     * @param[in] file The file descriptor to seek within.
     * @param[in] offset The offset within the file to seek to.
     * @param[in] whence The position to start seeking from.
     *
     * @return The new position in the file, or a negative error code.
     */
    off_t (*lseek)(struct file *file, off_t offset, int whence);
#endif

    /**
     * @brief Performs an I/O control operation on a file descriptor.
     *
     * @param[in] ioctl The file descriptor to control.
     * @param[in] request The type of I/O control request to perform.
     * @param[in] args Points to the variable argument block from user space.
     *
     * @return The result of the I/O control operation.
     */
    int (*ioctl)(struct file *file, unsigned long request, void *args);
};

/**
 * @brief Information about an open file descriptor.
 */
struct file
{
    /** Reference count on this file structure */
    file_ref_count_t count;

    /** Flags on the file; O_RDONLY, O_CREAT, etc */
    unsigned int flags;

    /** Permissions for the file */
    mode_t mode;

    /** Pointer to the operations table for the file descriptor */
    const struct file_operations *op;

    /** Pointer to the filesystem inode, or NULL if not in a filesystem */
    struct inode *inode;

    /** Extra information about the open file on FAT filesystems */
    struct fatfs_inode_info *fatfs_info;

    /** Current seek position in the file, if it is seekable */
    off_t posn;
};

/**
 * @brief Initializes the file descriptor subsystem.
 */
void file_init(void);

/**
 * @brief Adds a reference to an open file descriptor.
 *
 * @param[in] file The file to add a reference to.
 */
static inline void file_ref(struct file *file)
{
    ++(file->count);
}

/**
 * @brief Removes a reference from an open file descriptor and
 * closes it if there are no more references.
 *
 * @param[in] file The file to dereference.
 *
 * @return Returns zero on success, or an error code if something went
 * wrong during the close (the file descriptor will still be closed).
 */
int file_deref(struct file *file);

/**
 * @brief Gets the structure associated with a numeric file descriptor
 * in the current process.
 *
 * @param[in] fd The numeric file descriptor.
 *
 * @return The file descriptor structure, or NULL if @a fd is invalid.
 */
struct file *file_get(int fd);

/**
 * @brief Puts a file structure into the file descriptor table for the
 * current process.
 *
 * @param[in] file The file structure to put.
 *
 * @return Zero on success, or -EMFILE if there is no space in the
 * file descriptor table.
 */
int file_put(struct file *file);

/**
 * @brief Allocates a new file structure from the global file descriptor table.
 *
 * @param[in] flags The file open flags.
 * @param[in] mode The permission mode.
 *
 * @return A pointer to the new file structure, or NULL if the global file
 * descriptor table is full.
 *
 * The returned structure will have the reference count set to 1, and the
 * operation structure will be set to all-default handler functions.
 */
struct file *file_new(int flags, mode_t mode);

/**
 * @brief Opens a file.
 *
 * @param[in] path File pathname to open.
 * @param[in] flags Flags to use to open the file.
 * @param[in] mode Permission mode files, including the file type;
 * such as S_IFREG or S_IFDIR.
 *
 * @return File descriptor on success, or a negative error code otherwise.
 */
int file_open(const char *path, int flags, mode_t mode);

/**
 * @brief Default close function for a file descriptor.
 *
 * @param[in] file The file to close.
 *
 * @return Does nothing and always succeeds with a zero result.
 */
int file_close_default(struct file *file);

/**
 * @brief Default read function for a file descriptor.
 *
 * @param[in] file The file to read from.
 * @param[in] data Points to the buffer to read into.
 * @param[in] size The number of bytes to read.
 *
 * @return Always fails with -EINVAL, indicating that reading is disallowed.
 */
ssize_t file_read_default(struct file *file, void *data, size_t size);

/**
 * @brief Default write function for a file descriptor.
 *
 * @param[in] file The file descriptor to write to.
 * @param[in] data Points to the buffer to write from.
 * @param[in] size The number of bytes to write.
 *
 * @return Always fails with -EINVAL, indicating that writing is disallowed.
 */
ssize_t file_write_default(struct file *file, const void *data, size_t size);

#if CONFIG_LSEEK

/**
 * @brief Default seek function for a file descriptor.
 *
 * @param[in] file The file descriptor to seek within.
 * @param[in] offset The offset within the file to seek to.
 * @param[in] whence The position to start seeking from.
 *
 * @return Always fails with -ESPIPE, indicating that seeking is disallowed.
 */
off_t file_lseek_default(struct file *file, off_t offset, int whence);

/**
 * @brief Output a reference to the default lseek handler.
 *
 * Expands to nothing if lseek support has been disabled in the kernel.
 */
#define file_op_lseek_default .lseek = file_lseek_default,

#else

/* Disable the default lseek handler */
#define file_lseek_default NULL
#define file_op_lseek_default

#endif

/**
 * @brief Default I/O control operation on a file descriptor.
 *
 * @param[in] ioctl The file descriptor to control.
 * @param[in] request The type of I/O control request to perform.
 * @param[in] args Points to the variable argument block from user space.
 *
 * @return Always fails with -EINVAL, indicating that the request is
 * not supported by the file descriptor.
 */
int file_ioctl_default(struct file *file, unsigned long request, void *args);

#ifdef __cplusplus
}
#endif

#endif
