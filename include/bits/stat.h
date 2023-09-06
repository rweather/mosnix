/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#ifndef MOSNIX_BITS_STAT_H
#define MOSNIX_BITS_STAT_H

#define S_IFMT      0xF000      /**< File format mask */
#define S_IFIFO     0x1000      /**< FIFO */
#define S_IFCHR     0x2000      /**< Character device */
#define S_IFDIR     0x4000      /**< Directory */
#define S_IFBLK     0x6000      /**< Block device */
#define S_IFREG     0x8000      /**< Regular file */
#define S_IFLNK     0xA000      /**< Symbolic link */
#define S_IFSOCK    0xC000      /**< Socket */

#define S_ISUID     0x0800      /**< Set UID */
#define S_ISGID     0x0400      /**< Set GID */
#define S_ISVTX     0x0200      /**< Save text sticky bit */

#define S_IRWXU     0x01C0      /**< Mask for owner permissions */
#define S_IRUSR     0x0100      /**< Owner read */
#define S_IWUSR     0x0080      /**< Owner write */
#define S_IXUSR     0x0040      /**< Owner execute */

#define S_IRWXG     0x0038      /**< Mask for group permissions */
#define S_IRGRP     0x0020      /**< Group read */
#define S_IWGRP     0x0010      /**< Group write */
#define S_IXGRP     0x0008      /**< Group execute */

#define S_IRWXO     0x0007      /**< Mask for other permissions */
#define S_IROTH     0x0004      /**< Other read */
#define S_IWOTH     0x0002      /**< Other write */
#define S_IXOTH     0x0001      /**< Other execute */

#define S_IREAD     S_IRUSR     /**< Alias for owner read */
#define S_IWRITE    S_IWUSR     /**< Alias for owner write */
#define S_IEXEC     S_IXUSR     /**< Alias for owner execute */

#endif
