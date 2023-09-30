/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <mosnix/printk.h>
#include <mosnix/attributes.h>
#include <stdint.h>

static char const hexchars[] = "0123456789abcdef";

ATTR_NOINLINE void kputstr(const char *s)
{
    if (s) {
        char c;
        while ((c = *s++) != '\0') {
            kputchar(c);
        }
    }
}

ATTR_NOINLINE void kputquoted(const char *s, size_t limit)
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

ATTR_NOINLINE void kputhexbyte(unsigned char value)
{
    kputchar(hexchars[(value >> 4) & 0x0F]);
    kputchar(hexchars[value & 0x0F]);
    kputchar(' ');
}

ATTR_NOINLINE void kputhex(unsigned long value)
{
    kputchar('0');
    kputchar('x');
    kputchar(hexchars[(value >> 28) & 0x0F]);
    kputchar(hexchars[(value >> 24) & 0x0F]);
    kputchar(hexchars[(value >> 20) & 0x0F]);
    kputchar(hexchars[(value >> 16) & 0x0F]);
    kputchar(hexchars[(value >> 12) & 0x0F]);
    kputchar(hexchars[(value >> 8) & 0x0F]);
    kputchar(hexchars[(value >> 4) & 0x0F]);
    kputchar(hexchars[value & 0x0F]);
}

void kputptr(const void *value)
{
    kputhex((uintptr_t)value);
}
