/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#ifndef SHELL_PRINT_H
#define SHELL_PRINT_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Prints a single character to standard output.
 *
 * @param[in] c The character to print.
 */
void print_char(char c);

/**
 * @brief Prints a string to standard output.
 *
 * @param[in] s The string to print.
 */
void print_string(const char *s);

/**
 * @brief Prints a newline sequence to standard output.
 */
void print_nl(void);

#ifdef __cplusplus
}
#endif

#endif
