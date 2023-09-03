/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <mosnix/printk.h>
#include <stdint.h>

static char const hexchars[] = "0123456789abcdef";

__attribute__((noinline)) void kputstr(const char *s)
{
    if (s) {
        char c;
        while ((c = *s++) != '\0') {
            kputchar(c);
        }
    }
}

__attribute__((noinline)) void kputquoted(const char *s, size_t limit)
{
    if (s) {
        char c;
        kputchar('"');
        while (limit > 0 && (c = *s) != '\0') {
            if (c >= 0x20 && c <= 0x7F) {
                kputchar(c);
                --limit;
            } else {
                if (limit < 4)
                    break;
                kputchar('\\');
                kputchar('x');
                kputchar(hexchars[(c >> 4) & 0x0F]);
                kputchar(hexchars[c & 0x0F]);
                limit -= 4;
            }
            ++s;
        }
        if (limit > 0 && *s != '\0') {
            kputchar('.');
            kputchar('.');
        }
        kputchar('"');
    } else {
        kputstr("<null>");
    }
}

__attribute__((noinline)) void kputhex(unsigned int value)
{
    kputchar('0');
    kputchar('x');
    kputchar(hexchars[(value >> 12) & 0x0F]);
    kputchar(hexchars[(value >> 8) & 0x0F]);
    kputchar(hexchars[(value >> 4) & 0x0F]);
    kputchar(hexchars[value & 0x0F]);
}

void kputptr(const void *value)
{
    kputhex((uintptr_t)value);
}
