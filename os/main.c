/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <mosnix/proc.h>
#include <mosnix/sched.h>
#include <mosnix/syscall.h>
#include <stdio.h>

int main(void)
{
    /* Initialize all kernel subsystems */
    proc_init();

    /* Launch the shell process as our version of "init" */
    proc_start_shell();

    /* Run the scheduler forever */
    sched_start();
    return 0;
}
