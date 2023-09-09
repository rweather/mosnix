/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#ifndef MOSNIX_DIRENT_H
#define MOSNIX_DIRENT_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Maximum size of a name in "struct dirent" when the structure is
 * declared statically.  Inside libc, the name may be longer. */
#define DIRENT_NAME_MAX 16

/* Directory entry types */
#define DT_UNKNOWN      0
#define DT_FIFO         1
#define DT_CHR          2
#define DT_DIR          4
#define DT_BLK          6
#define DT_REG          8
#define DT_LNK          10
#define DT_SOCK         12

/* Information about a directory entry */
struct dirent
{
    ino_t d_ino;
    u_char d_type;
    char d_name[DIRENT_NAME_MAX];
};

/* Opaque type for directory reading */
typedef struct DIR_s DIR;

/* Directory functions */
extern DIR *opendir(const char *path);
extern int closedir(DIR *dir);
extern struct dirent *readdir(DIR *dir);

#ifdef __cplusplus
}
#endif

#endif
