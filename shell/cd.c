/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include "command.h"

/* For now, the home directory path is fixed in the shell to "/root" */
#define HOME_DIR "/root"

static char prev_path[PATH_MAX] = HOME_DIR;

int cmd_chdir(int argc, char **argv)
{
    const char *pathname;
    if (argc > 2) {
        print_stderr_string("cd: too many arguments\n");
        return 1;
    }
    getcwd(temp_path1, sizeof(temp_path1));
    if (argc == 1) {
        pathname = HOME_DIR;
    } else if (!strcmp(argv[1], "-")) {
        pathname = prev_path;
    } else {
        pathname = argv[1];
    }
    if (chdir(pathname) < 0) {
        print_error(pathname);
        return 1;
    } else {
        memcpy(prev_path, temp_path1, sizeof(prev_path));
        return 0;
    }
}
