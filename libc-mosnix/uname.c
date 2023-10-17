/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <sys/utsname.h>
#include <sys/syscall.h>
#include <string.h>
#include <errno.h>

int uname(struct utsname *buf)
{
    const struct utsname *buf2;
    if (!buf) {
        errno = EFAULT;
        return -1;
    } else if (syscall(SYS_getuname, &buf2) == 0) {
        memcpy(buf, buf2, sizeof(struct utsname));
        return 0;
    } else {
        return -1;
    }
}
