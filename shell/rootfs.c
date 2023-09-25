/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <unistd.h>
#include <sys/stat.h>
#include "print.h"

/* Generated automatically */

#define check_error(call) \
    do { \
        if ((call) < 0) \
            return -1; \
    } while (0)

int make_rootfs(void)
{
    check_error(mkdir("/bin", 0775));
    check_error(mkdir("/dev", 0775));
    check_error(mknod("/dev/console", 020660, 0x0501));
    check_error(mknod("/dev/full", 020660, 0x0107));
    check_error(mknod("/dev/mmcblk0", 060660, 0xb300));
    check_error(mknod("/dev/null", 020660, 0x0103));
    check_error(mknod("/dev/tty", 020660, 0x0500));
    check_error(mknod("/dev/tty0", 020660, 0x0400));
    check_error(mknod("/dev/zero", 020660, 0x0105));
    check_error(mkdir("/etc", 0775));
    check_error(mkdir("/home", 0775));
    check_error(mkdir("/mnt", 0775));
    check_error(mkdir("/mnt/other", 0775));
    check_error(mkdir("/mnt/sd", 0775));
    /*check_error(mount("/dev/mmcblk0", "/mnt/sd", "fat32", 0, 0));*/
    check_error(mkdir("/root", 0700));
    check_error(mkdir("/tmp", 0775));
    check_error(mkdir("/usr", 0775));
    check_error(mkdir("/var", 0775));
    return 0;
}
