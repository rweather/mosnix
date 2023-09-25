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

/**
 * @brief Flushes standard output.
 */
void print_flush(void);

/**
 * @brief Prints a formatted number.
 *
 * @param[in] value The numeric value to print.
 * @param[in] size The size of the field, for left-padding with spaces.
 */
void print_number(unsigned long value, unsigned char size);

/**
 * @brief Prints a single character to standard error.
 *
 * @param[in] c The character to print.
 */
void print_stderr_char(char c);

/**
 * @brief Prints a string to standard error.
 *
 * @param[in] s The string to print.
 */
void print_stderr_string(const char *s);

/**
 * @brief Prints a newline sequence to standard error.
 */
void print_stderr_nl(void);

/**
 * @brief Prints a pathname and errno to standard error.
 *
 * @param[in] pathname The pathname.
 */
void print_error(const char *pathname);

#ifdef __cplusplus
}
#endif

#endif
