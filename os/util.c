/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <mosnix/util.h>
#include <mosnix/attributes.h>
#include <bits/fcntl.h>
#include <unistd.h>

ATTR_NOINLINE size_t strlen_constrained(const char *s, size_t max)
{
    size_t len = 0;
    if (s) {
        while (max > 0) {
            if (*s++ == '\0')
                break;
            ++len;
            --max;
        }
    }
    return len;
}

ATTR_NOINLINE void strcpy_constrained(char *dest, const char *src, size_t max)
{
    if (src) {
        char c;
        while (max > 1) {
            c = *src++;
            if (c == '\0')
                break;
            *dest++ = c;
            --max;
        }
    }
    dest[0] = '\0';
}

ATTR_NOINLINE int str_is_too_long(const char *s, size_t max)
{
    while (max > 0) {
        if (*s++ == '\0')
            return 0;
        --max;
    }
    return 1;
}

ATTR_NOINLINE size_t strlen_path_component(const char *s, size_t max)
{
    size_t len = 0;
    char c;
    while (max > 0) {
        c = *s++;
        if (c == '/' || c == '\0')
            break;
        ++len;
        --max;
    }
    return len;
}

int oflag_to_access_mode(int oflag)
{
    switch (oflag & O_ACCMODE) {
    case O_RDONLY:  return R_OK;
    case O_WRONLY:  return W_OK;
    case O_RDWR:    return R_OK | W_OK;
    /* If oflag is 3, then assume we are trying to execute the file */
    default:        return X_OK;
    }
}
