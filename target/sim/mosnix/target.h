/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#ifndef MOSNIX_TARGET_SIM_H
#define MOSNIX_TARGET_SIM_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SYS_CLOCKS_PER_SEC 1000000UL
extern clock_t sys_clock(void);

/* mos-sim target uses the basic tty driver as its console */
#define CONFIG_CONSOLE_BASIC_TTY 1
/* mos-sim echos input characters whether we want it or not */
#define CONFIG_CHRIN_ECHO 1
/* mos-sim handles LF to CRLF conversion for us */
#define CONFIG_CHROUT_LF_HANDLING 1
/* mos-sim does not support __chrin_no_wait() */
#define CONFIG_CHROUT_NO_WAIT 0
extern int getchar(void);
extern void __putchar(char c);
#define __chrin() (getchar())
#define __chrout(c) (__putchar((c)))

#ifdef __cplusplus
}
#endif

#endif
