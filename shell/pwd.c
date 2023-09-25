/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include "command.h"
#include <mosnix/config.h>

int cmd_pwd(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    if (getcwd(temp_path1, sizeof(temp_path1)) != NULL) {
        print_string(temp_path1);
        print_nl();
        return 0;
    } else {
        print_error("pwd");
        return 1;
    }
}
