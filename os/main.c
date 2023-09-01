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

extern void run_scheduler(void);

int main(void)
{
    /* Print a welcome banner */
    puts("MOSnix " MOSNIX_VERSION);

    /* Initialize all kernel subsystems */
    proc_init();

    /* Launch the shell process */
    /* TODO */

    /* Run the scheduler forever */
    // TODO
    //run_scheduler();
    return 0;
}
