/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <mosnix/syscall.h>
#include <mosnix/proc.h>
#include <errno.h>

int sys_getuid(void)
{
    return current_proc->uid;
}

int sys_geteuid(void)
{
    return current_proc->euid;
}

int sys_setuid(struct sys_setuid_s *args)
{
    /* Not supported at present */
    (void)args;
    return -EPERM;
}

int sys_seteuid(struct sys_seteuid_s *args)
{
    /* Not supported at present */
    (void)args;
    return -EPERM;
}

int sys_getgid(void)
{
    return current_proc->gid;
}

int sys_getegid(void)
{
    return current_proc->egid;
}

int sys_setgid(struct sys_setgid_s *args)
{
    /* Not supported at present */
    (void)args;
    return -EPERM;
}

int sys_setegid(struct sys_setegid_s *args)
{
    /* Not supported at present */
    (void)args;
    return -EPERM;
}
