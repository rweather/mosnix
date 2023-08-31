/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <mosnix/syscall.h>
#include <errno.h>

int sys_getpid(void)
{
    /* TODO */
    return 1;
}

void sys_exit(struct sys_exit_s *args)
{
    /* TODO */
    (void)args;
}
