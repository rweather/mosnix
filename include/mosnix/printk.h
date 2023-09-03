/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#ifndef MOSNIX_PRINTK_H
#define MOSNIX_PRINTK_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Put a single character to the kernel's console output.
 *
 * @param[in] c The character to put.
 */
#define kputchar(c) (__putchar((c)))

/**
 * @brief Puts a newline sequence to the kernel's console output.
 */
#define kputnl() (__putchar('\n'))

/**
 * @brief Puts a NUL-terminated string to the kernel's console output.
 *
 * @param[in] s The string to put.
 *
 * Unlike the system puts(), this will not terminate @a s with a
 * newline sequence.  Put the newline in the string if you need it,
 * or call kputnl() instead.
 */
void kputstr(const char *s);

/**
 * @brief Puts a quoted NUL-terminated string to the kernel's console output.
 *
 * @param[in] s The string to put.
 * @param[in] limit Maximum number of characters to put before giving up,
 * just in case the string is not NUL terminated.
 *
 * The string will be surrounded by double quoted and non-printable
 * ASCII characters will be quoted with backslash escapes.  This function
 * is thus safe to use when printing application-supplied strings
 * like filenames.
 */
void kputquoted(const char *s, size_t limit);

/**
 * @brief Puts a hexadecimal value to the kernel's console output.
 *
 * @param[in] value The value to put in hexadecimal.
 */
void kputhex(unsigned int value);

/**
 * @brief Puts a pointer value to the kernel's console output.
 *
 * @param[in] ptr The pointer value to put in hexadecimal.
 */
void kputptr(const void *value);

/**
 * @brief Full printf() implementation that writes to the kernel's
 * console output.
 *
 * This should usually be avoided because it sucks in a lot of dependencies.
 * It is however useful for diagnostics when tracking down a problem.
 */
#define printk printf

#ifdef __cplusplus
}
#endif

#endif
