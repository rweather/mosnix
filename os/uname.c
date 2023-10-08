/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <mosnix/syscall.h>
#include <mosnix/target.h>
#include <string.h>
#include <errno.h>

static struct utsname const uname_data = {
    "MOSnix",
    "mosnix",
    MOSNIX_VERSION,
    CONFIG_TARGET_NAME,
#if defined(CPU_65C02)
    "mos65c02"
#else
    "mos6502"
#endif
};

int sys_uname(struct sys_uname_s *args)
{
    struct utsname *buf = args->buf;
    if (!buf)
        return -EFAULT;
    memcpy(buf, &uname_data, sizeof(uname_data));
    return 0;
}
