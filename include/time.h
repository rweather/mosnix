/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#ifndef MOSNIX_TIME_H
#define MOSNIX_TIME_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CLOCK_REALTIME  0
#define CLOCK_MONOTONIC 1

time_t time(time_t *timep);
int clock_gettime(clockid_t clockid, struct timespec *ts);
int clock_settime(clockid_t clockid, struct timespec *ts);

extern char *tzname[2];
extern long timezone;
extern int daylight;
void tzset(void);

#ifdef __cplusplus
}
#endif

#endif
