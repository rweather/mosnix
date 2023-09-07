/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#ifndef MOSNIX_DEVNUM_H
#define MOSNIX_DEVNUM_H

#include <sys/sysmacros.h>

/*
 * When allocating new device numbers, it is preferable to use numbers
 * from the following list:
 *
 * https://www.kernel.org/doc/Documentation/admin-guide/devices.txt
 */

/* Major device numbers */
enum {
    DEV_MAJOR_UNNAMED       = 0,    /**< Unnamed devices */
    DEV_MAJOR_MEMORY        = 1,    /**< Memory devices */
    DEV_MAJOR_TTY           = 4,    /**< TTY devices */
    DEV_MAJOR_TTY_ALT       = 5,    /**< Alternate TTY devices */
    DEV_MAJOR_MMC           = 179   /**< SD/MMC devices */
};

/* Character memory devices */
#define DEV_NULL makedev(DEV_MAJOR_MEMORY, 3) /**< /dev/null */
#define DEV_ZERO makedev(DEV_MAJOR_MEMORY, 5) /**< /dev/zero */
#define DEV_FULL makedev(DEV_MAJOR_MEMORY, 7) /**< /dev/full */

/* TTY devices */
#define DEV_TTY0    makedev(DEV_MAJOR_TTY, 0)     /**< /dev/tty0 */
#define DEV_TTY     makedev(DEV_MAJOR_TTY_ALT, 0) /**< /dev/tty */
#define DEV_CONSOLE makedev(DEV_MAJOR_TTY_ALT, 1) /**< /dev/console */

/* SD/MMC devices */
#define DEV_MMCBLK0 makedev(DEV_MAJOR_MMC, 0) /**< /dev/mmcblk0 */

#endif
