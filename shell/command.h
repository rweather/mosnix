/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#ifndef SHELL_COMMAND_H
#define SHELL_COMMAND_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include "print.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Maximum pathname size, including the NUL terminator.
 *
 * Must match the value in <mosnix/config.h>.
 */
#define PATH_MAX 64

/**
 * @brief Temporary variable for composing a pathname during a command.
 */
extern char temp_path1[PATH_MAX];

/**
 * @brief Second temporary variable for composing a pathname during a command.
 */
extern char temp_path2[PATH_MAX];

/**
 * @brief Function prototype for a builtin command handler.
 *
 * @param[in] argc Number of arguments that were passed to the command.
 * @param[in] argv List of argument strings.
 *
 * @return The exit status, 0 on success, non-zero on failure.
 */
typedef int (*builtin_handler_t)(int argc, char **argv);

/**
 * @brief Find the handler for a builtin command.
 *
 * @return[in] name The name of the command from the command-line.
 *
 * @return The handler pointer, or NULL if @a name is not a builtin.
 */
builtin_handler_t cmd_find_builtin(const char *name);

/**
 * @brief Parses and executes a shell command.
 *
 * @param[in] line Points to the command-line to be parsed.
 */
void cmd_exec(char *line);

/* Command handlers */
int cmd_chdir(int argc, char **argv);
int cmd_ls(int argc, char **argv);
int cmd_pwd(int argc, char **argv);
int cmd_uname(int argc, char **argv);

#ifdef __cplusplus
}
#endif

#endif
