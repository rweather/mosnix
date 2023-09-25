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
    "Operation not permitted",
    "No such file or directory",
    "No such process",
    "Interrupted system call",
    "I/O error",
    "No such device or address",
    "Argument list too long",
    "Exec format error",
    "Bad file number",
    "No child processes",
    "Try again",
    "Out of memory",
    "Permission denied",
    "Bad address",
    "Block device required",
    "Device or resource busy",
    "File exists",
    "EXDEV",
    "No such device",
    "Not a directory",
    "Is a directory",
    "Invalid argument",
    "File table overflow",
    "Too many open files",
    "Not a typewriter",
    "ETXTBSY",
    "File too large",
    "No space left on device",
    "Illegal seek",
    "Read-only file system",
    "EMLINK",
    "EPIPE",
    "EDOM",
    "ERANGE",
    "EDEADLK",
    "File name too long",
    "ENOLCK",
    "Invalid system call",
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