/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <mosnix/file.h>
#include <mosnix/kmalloc.h>
#include <mosnix/proc.h>
#include <mosnix/sched.h>
#include <mosnix/syscall.h>
#include <mosnix/target.h>
#include "drivers/sdcard/sdcard.h"
#include "fs/ram/ramfs.h"
#include <stdio.h>

int main(void)
{
    /* Initialize all kernel subsystems */
    kmalloc_init();
    ramfs_init();
    file_init();
#if defined(CONFIG_SD) && defined(CONFIG_SPI)
    sd_init();
    sd_detect();
#endif
    proc_init();

    /* Launch the shell process as our version of "init" */
    proc_start_shell();

    /* Run the scheduler forever */
    sched_start();
    return 0;
}
