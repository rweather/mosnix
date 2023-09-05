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

void print_char(char c)
{
    write(1, &c, 1);
}

void print_string(const char *s)
{
    if (s)
        write(1, s, strlen(s));
}

void print_nl(void)
{
    write(1, "\n", 1);
}
