/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#ifndef MOSNIX_SYS_MOUNT_H
#define MOSNIX_SYS_MOUNT_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Mount flags.
 *
 * Most of these are ignored by MOSnix, provided for source compatibility.
 */
#define MS_RDONLY           0x00000001UL
#define MS_NOSUID           0x00000002UL
#define MS_NODEV            0x00000004UL
#define MS_NOEXEC           0x00000008UL
#define MS_SYNCHRONOUS      0x00000010UL
#define MS_REMOUNT          0x00000020UL
#define MS_MANDLOCK         0x00000040UL
#define MS_DIRSYNC          0x00000080UL
#define MS_NOSYMFOLLOW      0x00000100UL
#define MS_NOATIME          0x00000400UL
#define MS_NODIRATIME       0x00000800UL
#define MS_BIND             0x00001000UL
#define MS_MOVE             0x00002000UL
#define MS_REC              0x00004000UL
#define MS_SILENT           0x00008000UL
#define MS_POSIXACL         0x00010000UL
#define MS_UNBINDABLE       0x00020000UL
#define MS_PRIVATE          0x00040000UL
#define MS_SLAVE            0x00080000UL
#define MS_SHARED           0x00100000UL
#define MS_RELATIME         0x00200000UL
#define MS_KERNMOUNT        0x00400000UL
#define MS_I_VERSION        0x00800000UL
#define MS_STRICTATIME      0x01000000UL
#define MS_LAZYTIME         0x02000000UL
#define MS_ACTIVE           0x40000000UL
#define MS_NOUSER           0x80000000UL

/* Mount a filesystem */
int mount(const char *source, const char *target, const char *filesystemtype,
          unsigned long mountflags, const void *data);

/* Unmount a filesystem */
int umount(const char *target);

#ifdef __cplusplus
}
#endif

#endif
