/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include "print.h"
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <mosnix/attributes.h>

ATTR_NOINLINE void print_char(char c)
{
    write(1, &c, 1);
}

ATTR_NOINLINE void print_string(const char *s)
{
    if (s)
        write(1, s, strlen(s));
}

ATTR_NOINLINE void print_nl(void)
{
    write(1, "\n", 1);
}

ATTR_NOINLINE void print_flush(void)
{
    /* TODO */
}

extern unsigned char div10(unsigned long *value);

ATTR_NOINLINE void print_number(unsigned long value, unsigned char size)
{
    char buf[size];
    unsigned char count = 0;
    while (count < 10 && count < size && value != 0) {
        buf[count] = div10(&value) + '0';
        ++count;
    }
    if (count == 0) {
        buf[count++] = '0';
    }
    while (count < size) {
        buf[count++] = ' ';
    }
    while (count > 0) {
        --count;
        print_char(buf[count]);
    }
}

ATTR_NOINLINE void print_stderr_char(char c)
{
    write(2, &c, 1);
}

ATTR_NOINLINE void print_stderr_string(const char *s)
{
    if (s)
        write(2, s, strlen(s));
}

ATTR_NOINLINE void print_stderr_nl(void)
{
    write(2, "\n", 1);
}

ATTR_NOINLINE void print_error(const char *pathname)
{
    if (pathname) {
        print_stderr_string(pathname);
        print_stderr_string(": ");
    }
    print_stderr_string(strerror(errno));
    print_stderr_nl();
}
