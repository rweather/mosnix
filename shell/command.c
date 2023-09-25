/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include "command.h"
#include <mosnix/config.h>
#include <mosnix/attributes.h>

char temp_path1[PATH_MAX];
char temp_path2[PATH_MAX];

struct builtin_command
{
    const char *name;
    builtin_handler_t handler;
};

/** List of all builtin commands, which must be sorted alphanumerically */
static struct builtin_command const builtins[] = {
    {"cd",          cmd_chdir},
    {"ls",          cmd_ls},
    {"pwd",         cmd_pwd},
    {"uname",       cmd_uname},
};

builtin_handler_t cmd_find_builtin(const char *name)
{
    int left = 0;
    int right = sizeof(builtins) / sizeof(builtins[0]);
    int middle, cmp;
    while (left <= right) {
        middle = (left + right) / 2;
        cmp = strcmp(name, builtins[middle].name);
        if (cmp < 0) {
            right = middle - 1;
        } else if (cmp > 0) {
            left = middle + 1;
        } else {
            return builtins[middle].handler;
        }
    }
    return 0;
}

static int cmd_isspace(char ch)
{
    return ch == ' ' || (ch >= 0x09 && ch <= 0x0D);
}

void cmd_exec(char *line)
{
    static char *argv[CONFIG_ARGC_MAX + 1] ATTR_SECTION_NOINIT;
    int argc = 0;
    char *out;
    char quote = 0;
    char ch;
    builtin_handler_t builtin;

    /* Parse the command-line into whitespace-separated arguments */
    for (;;) {
        /* Skip whitespace between arguments */
        while ((ch = *line) != '\0' && cmd_isspace(ch)) {
            ++line;
        }
        if (ch == '\0' || ch == '#') {
            break;
        }

        /* If we have too many arguments, then bail out */
        if (argc >= CONFIG_ARGC_MAX) {
            print_stderr_string("sh: too many arguments\n");
            return;
        }

        /* Extract the next argument, removing quotes as we go */
        argv[argc] = out = line;
        while ((ch = *line) != '\0') {
            if (cmd_isspace(ch) && quote == 0) {
                ++line;
                break;
            } else if (ch == '\'' || ch == '"') {
                if (quote == 0) {
                    quote = ch;
                } else if (quote == ch) {
                    quote = 0;
                } else {
                    *out++ = ch;
                }
            } else if (ch == '\\' && quote != '\'') {
                if (line[1] == '\0') {
                    *out++ = ch;
                } else {
                    *out++ = line[1];
                    ++line;
                }
            } else {
                *out++ = ch;
            }
            ++line;
        }
        if (quote != 0) {
            print_stderr_string("sh: unterminated quote\n");
            return;
        }
        *out = '\0';
        ++argc;
    }
    if (argc == 0) {
        /* Command is empty, so nothing to do */
        return;
    }
    argv[argc] = 0;

    /* Try to find a builtin command for argv[0] */
    builtin = cmd_find_builtin(argv[0]);
    if (builtin) {
        optind = 0; /* Reset option parsing for the builtin command */
        builtin(argc, argv);
        return;
    }

    /* Execute an external command */
    /* TODO */
    print_stderr_string(argv[0]);
    print_stderr_string(": command not found\n");
}
