/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include "print.h"
#include "command.h"
#include "fstab.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/mount.h>

int make_rootfs(void);

static char buffer[128];

static int get_line(char *buf, size_t size)
{
    size_t posn = 0;
    int ch;
    for (;;) {
        ch = getchar();
        if (ch < 0) {
            return 0;
        } else if (ch == '\r' || ch == '\n') {
            break;
        } else if (ch == '\b' || ch == 0x7F) {
            if (posn > 0) {
                --posn;
            }
        } else if (ch == 0x15) { /* CTRL-U */
            posn = 0;
        } else if (ch >= ' ' && posn < (size - 1U)) {
            buf[posn++] = (char)ch;
        }
    }
    buf[posn] = '\0';
    return 1;
}

/**
 * @brief Perform system initialization tasks.
 */
void init(void)
{
    struct timespec ts;
    int result;

    /* Set the full umask so that the root filesystem nodes are
     * created with exactly the mode bits we want. */
    umask(0);

    /* Make the root filesystem skeleton */
    print_string("Mounting root filesystem ... ");
    print_flush();
    if (make_rootfs() < 0) {
        print_error(NULL);
    } else {
        add_fstab("/", "ramfs", "ramfs", "rw");
        print_string("ok\n");
    }

    /* Mount the SD card if we have the hardware for it */
    if (mount("/dev/mmcblk0", NULL, "vfat", 0, 0) == 0) {
        print_string("Mounting SD card ... ");
        print_flush();
        result = mount("/dev/mmcblk0", "/mnt/sd", "vfat", 0, 0);
        if (result < 0) {
            print_error(NULL);
        } else if (result == 1) {
            /* Non-POSIX: Special result code that means "No card found".
             * The filesystem is still mounted, awaiting card insertion. */
            print_string("No card found\n");
        } else {
            print_string("ok\n");
        }
        if (result >= 0) {
            add_fstab("/mnt/sd", "/dev/mmcblk0", "vfat", "ro");
        }
    }

    /* Revert to a normal umask for shell operations */
    umask(022);

    /* Print the boot time in seconds */
    clock_gettime(CLOCK_MONOTONIC, &ts);
    print_string("Boot time ... ");
    if (ts.tv_nsec >= 500000000L)
        ++(ts.tv_sec);
    print_number(ts.tv_sec, 2);
    print_string("s\n");
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    /* Print the OS and machine name at startup */
    print_nl();
    cmd_uname(0, 0);
    print_nl();

    /* Initialize the system */
    init();
    print_nl();

    /* Main loop for the shell */
    for (;;) {
        /* Print the current working directory and the prompt */
        getcwd(temp_path1, sizeof(temp_path1));
        print_string(temp_path1);
        print_string("# ");

        /* Get a line of input */
        if (!get_line(buffer, sizeof(buffer)))
            break;

        /* Process the input line */
        cmd_exec(buffer);
    }
    print_nl();

    return 0;
}
