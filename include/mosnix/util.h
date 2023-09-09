/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#ifndef MOSNIX_UTIL_H
#define MOSNIX_UTIL_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Constrained version of strlen() that won't go beyond a
 * given maximum length.
 *
 * @param[in] s The string, which can be NULL.
 * @param[in] max The maximum length that may be reported.
 *
 * @return The length of the string.
 */
size_t strlen_constrained(const char *s, size_t max);

/**
 * @brief Copies a string into a constrained destination buffer.
 *
 * @param[out] dest Destination buffer, which must not be NULL.
 * @param[in] src Source string to copy, which can be NULL.
 * @param[in] max Maximum number of characters to copy to @a dest,
 * including the terminating NUL.  Must be greater than or equal to 1.
 *
 * This is safer than strncpy() because it guarantees to NUL-terminate
 * the destination even if @a src is @a max characters in length.
 * And it will not fail when @a src is NULL.
 */
void strcpy_constrained(char *dest, const char *src, size_t max);

/**
 * @brief Determine if a string is too long.
 *
 * @param[in] s The string, which most not be NULL.
 * @param[in] max The maximum number of characters to search.
 *
 * @return Non-zero if there is no NUL before @a max characters, or zero
 * if the string is OK.
 */
int str_is_too_long(const char *s, size_t max);

/**
 * @brief Gets the length of a path component, terminated by '/' or '\0'.
 *
 * @param[in] s The string, which most not be NULL.
 * @param[in] max The maximum length that may be searched.
 *
 * @return The length of the path component.
 */
size_t strlen_path_component(const char *s, size_t max);

/**
 * @brief Converts a set of O_* flags to a mode suitable for access().
 *
 * @param[in] oflag The open mode.
 *
 * @return The access mode.
 */
int oflag_to_access_mode(int oflag);

#ifdef __cplusplus
}
#endif

#endif
