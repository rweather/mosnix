/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <errno.h>
#include <mosnix/syscall.h>

/* Generated automatically */

static const char * const errlist[] = {
    "No error",
    "Not permitted",
    "No such file or directory",
    "No such process",
    "EINTR",
    "I/O error",
    "No such device",
    "Arg list too long",
    "Exec format error",
    "Bad file number",
    "No child processes",
    "Try again",
    "Out of memory",
    "Permission denied",
    "Bad address",
    "Block device required",
    "Busy",
    "File exists",
    "EXDEV",
    "No such device",
    "Not a directory",
    "Is a directory",
    "Invalid argument",
    "File table overflow",
    "Too many open files",
    "ENOTTY",
    "ETXTBSY",
    "File too large",
    "No space left",
    "ESPIPE",
    "Read-only",
    "EMLINK",
    "EPIPE",
    "EDOM",
    "ERANGE",
    "EDEADLK",
    "Name too long",
    "ENOLCK",
    "Bad syscall",
    "Directory not empty",
    "ELOOP",
};
#define errno_count ((int)(sizeof(errlist) / sizeof(errlist[0])))

int sys_strerror(struct sys_strerror_s *args)
{
    int errnum = args->errnum;
    if (errnum >= 0 && errnum < errno_count)
        *(args->result) = (char *)errlist[errnum];
    else
        *(args->result) = (char *)"Error";
    return 0;
}
