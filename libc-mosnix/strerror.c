/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <errno.h>
#include <sys/syscall.h>
#include <string.h>

const char *strerror(int errnum)
{
    /* The error messages are inside the kernel so that each process
     * doesn't need its own copy of the error message table. */
    char *result = 0;
    syscall(SYS_strerror, errnum, &result);
    return result;
}
