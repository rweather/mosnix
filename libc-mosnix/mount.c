/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <sys/mount.h>
#include <sys/syscall.h>

int mount(const char *source, const char *target, const char *filesystemtype,
          unsigned long mountflags, const void *data)
{
    return syscall(SYS_mount, source, target, filesystemtype, mountflags, data);
}

int umount(const char *target)
{
    return syscall(SYS_umount, target);
}
