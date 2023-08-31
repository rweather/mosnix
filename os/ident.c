/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <mosnix/syscall.h>
#include <errno.h>

/* For now, everyone is root.  Please Unix responsibly! */

static int sys_getid(void)
{
    return 0;
}

static int sys_setid(id_t id)
{
    if (id != 0)
        return -EINVAL;
    else
        return 0;
}

int sys_getuid(void)
{
    return sys_getid();
}

int sys_geteuid(void)
{
    return sys_getid();
}

int sys_setuid(struct sys_setuid_s *args)
{
    return sys_setid(args->uid);
}

int sys_seteuid(struct sys_seteuid_s *args)
{
    return sys_setid(args->uid);
}

int sys_getgid(void)
{
    return sys_getid();
}

int sys_getegid(void)
{
    return sys_getid();
}

int sys_setgid(struct sys_setgid_s *args)
{
    return sys_setid(args->gid);
}

int sys_setegid(struct sys_setegid_s *args)
{
    return sys_setid(args->gid);
}
