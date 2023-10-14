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

/* Name of the target for uname */
#define CONFIG_TARGET_NAME "eater"

/* Get the monotonic system clock in 1/256'ths of a second */
extern void sys_monoclock(long long *t);

/* Get the low 16 bits of the monotonic system clock */
extern unsigned short sys_clock(void);

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

/* eater target may have an SPI interface on PORTA of the VIA */
#define CONFIG_SPI 1
/* eater target may have an SD card module on the SPI interface */
#define CONFIG_SD 1

#ifdef __cplusplus
}
#endif

#endif
