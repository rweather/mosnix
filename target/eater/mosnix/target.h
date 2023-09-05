/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#ifndef MOSNIX_TARGET_EATER_H
#define MOSNIX_TARGET_EATER_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SYS_CLOCKS_PER_SEC 1000UL
extern clock_t sys_clock(void);

/* eater target uses the basic tty driver as its console */
#define CONFIG_CONSOLE_BASIC_TTY 1
/* eater target does not automatically echo */
#define CONFIG_CHRIN_ECHO 0
/* eater target does not automaticaly do LF to CRLF conversion */
#define CONFIG_CHROUT_LF_HANDLING 0
/* eater target supports __chrin_no_wait() */
#define CONFIG_CHROUT_NO_WAIT 1
int __chrin(void);
int __chrin_no_wait(void);
void __chrout(char c);

#ifdef __cplusplus
}
#endif

#endif
