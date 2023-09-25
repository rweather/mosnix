/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#ifndef MOSNIX_UNISTD_H
#define MOSNIX_UNISTD_H

#include <bits/unistd.h>

/* Modes for access() */
#define F_OK 0
#define X_OK 1
#define W_OK 2
#define R_OK 3

/* Modes for lseek */
#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif

#endif
