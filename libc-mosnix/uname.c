/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <sys/utsname.h>
#include <sys/syscall.h>

int uname(struct utsname *buf)
{
    return syscall(SYS_uname, buf);
}
