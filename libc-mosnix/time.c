/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <time.h>
#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>

/* The kernel provides a monotonic tick counter in 1/256's of a second.
 * The kernel also provides a real time tick "offset".  To get the current
 * real time, add the offset to the monotonic tick value. */

time_t time(time_t *timep)
{
    long long t1;
    long long t2;
    time_t t;
    syscall(SYS_getmonotime, &t1);
    syscall(SYS_getrealtime, &t2);
    t = (time_t)((t1 + t2) >> 8);
    if (timep)
        *timep = t;
    return t;
}

int clock_gettime(clockid_t clockid, struct timespec *ts)
{
    long long t1;
    long long t2;
    if (!ts) {
        errno = EFAULT;
        return -1;
    } else if (clockid == CLOCK_MONOTONIC) {
        syscall(SYS_getmonotime, &t1);
    } else if (clockid == CLOCK_REALTIME) {
        syscall(SYS_getmonotime, &t1);
        syscall(SYS_getrealtime, &t2);
        t1 += t2;
        if (t1 < 0)
            t1 = 0; /* Cannot go backwards past the epoch */
    } else {
        errno = EINVAL;
        return -1;
    }
    ts->tv_sec = (time_t)(t1 >> 8);
    ts->tv_nsec = (long)(((t1 & 0xFF) * 1000000000LL) >> 8);
    return 0;
}

int clock_settime(clockid_t clockid, struct timespec *ts)
{
    if (!ts) {
        errno = EFAULT;
        return -1;
    } else if (clockid == CLOCK_REALTIME &&
               ts->tv_sec >= 0 && ts->tv_nsec >= 0) {
        long long t1;
        long long t2;
        t2 = (((long long)(ts->tv_sec)) << 8);
        t2 += (((long long)(ts->tv_nsec)) << 8) / 1000000000LL;
        syscall(SYS_getmonotime, &t1);
        t2 -= t1;
        return syscall(SYS_setrealtime, t2);
    } else {
        errno = EINVAL;
        return -1;
    }
}

char *tzname[2] = {"UTC", "UTC"};
long timezone = 0;
int daylight = 0;

void tzset(void)
{
    /* Not implemented yet - default is UTC only */
}
