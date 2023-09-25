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

#if CONFIG_ACCESS_UID

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

#else /* !CONFIG_ACCESS_UID */

/* Access checks are disabled; the only user in the system is root */

SYS_ATTR int sys_getid(void)
{
    return 0;
}

SYS_ATTR int sys_setid(void)
{
    return -ENOSYS;
}

int sys_getuid(void)  __attribute__((alias("sys_getid")));
int sys_geteuid(void) __attribute__((alias("sys_getid")));
int sys_getgid(void)  __attribute__((alias("sys_getid")));
int sys_getegid(void) __attribute__((alias("sys_getid")));

int sys_setuid(struct sys_setuid_s *args)   __attribute__((alias("sys_setid")));
int sys_seteuid(struct sys_seteuid_s *args) __attribute__((alias("sys_setid")));
int sys_setgid(struct sys_setgid_s *args)   __attribute__((alias("sys_setid")));
int sys_setegid(struct sys_setegid_s *args) __attribute__((alias("sys_setid")));

#endif /* !CONFIG_ACCESS_UID */
