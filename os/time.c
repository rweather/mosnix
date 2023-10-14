/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <mosnix/syscall.h>
#include <mosnix/target.h>
#include <mosnix/inode.h>
#include <mosnix/attributes.h>

/** Offset that converts monotonic time into real time */
static long long realtime = 0;

int sys_getmonotime(struct sys_getmonotime_s *args)
{
    sys_monoclock(args->t);
    return 0;
}

int sys_getrealtime(struct sys_getrealtime_s *args)
{
    *(args->t) = realtime;
    return 0;
}

int sys_setrealtime(struct sys_setrealtime_s *args)
{
    realtime = args->t;
    return 0;
}

ATTR_NOINLINE time_t inode_get_mtime(void)
{
    long long t;
    sys_monoclock(&t);
    return (time_t)((t + realtime) >> 8);
}
