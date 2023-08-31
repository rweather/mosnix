/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <mosnix/proc.h>
#include <mosnix/syscall.h>
#include <stdio.h>

int main(void)
{
    /* Print a welcome banner */
    puts("MOSnix " MOSNIX_VERSION);

    /* Initialize all kernel subsystems */
    proc_init();

    /* TODO */
    return 0;
}
