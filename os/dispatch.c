/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <mosnix/syscall.h>

/* Generated automatically */

void * const SYSCALL_TABLE[128] __attribute__((retain)) = {
    /*   0 */ (void *)sys_read,
    /*   1 */ (void *)sys_write,
    /*   2 */ (void *)sys_open,
    /*   3 */ (void *)sys_close,
    /*   4 */ (void *)sys_lseek,
    /*   5 */ (void *)sys_fcntl,
    /*   6 */ (void *)sys_dup,
    /*   7 */ (void *)sys_dup2,
    /*   8 */ (void *)sys_notimp,
    /*   9 */ (void *)sys_notimp,
    /*  10 */ (void *)sys_notimp,
    /*  11 */ (void *)sys_notimp,
    /*  12 */ (void *)sys_notimp,
    /*  13 */ (void *)sys_notimp,
    /*  14 */ (void *)sys_notimp,
    /*  15 */ (void *)sys_notimp,
    /*  16 */ (void *)sys_notimp,
    /*  17 */ (void *)sys_notimp,
    /*  18 */ (void *)sys_notimp,
    /*  19 */ (void *)sys_notimp,
    /*  20 */ (void *)sys_getcwd,
    /*  21 */ (void *)sys_chdir,
    /*  22 */ (void *)sys_notimp,
    /*  23 */ (void *)sys_notimp,
    /*  24 */ (void *)sys_notimp,
    /*  25 */ (void *)sys_notimp,
    /*  26 */ (void *)sys_notimp,
    /*  27 */ (void *)sys_notimp,
    /*  28 */ (void *)sys_notimp,
    /*  29 */ (void *)sys_notimp,
    /*  30 */ (void *)sys_notimp,
    /*  31 */ (void *)sys_notimp,
    /*  32 */ (void *)sys_notimp,
    /*  33 */ (void *)sys_notimp,
    /*  34 */ (void *)sys_notimp,
    /*  35 */ (void *)sys_notimp,
    /*  36 */ (void *)sys_notimp,
    /*  37 */ (void *)sys_notimp,
    /*  38 */ (void *)sys_notimp,
    /*  39 */ (void *)sys_notimp,
    /*  40 */ (void *)sys_notimp,
    /*  41 */ (void *)sys_notimp,
    /*  42 */ (void *)sys_notimp,
    /*  43 */ (void *)sys_notimp,
    /*  44 */ (void *)sys_notimp,
    /*  45 */ (void *)sys_notimp,
    /*  46 */ (void *)sys_notimp,
    /*  47 */ (void *)sys_notimp,
    /*  48 */ (void *)sys_notimp,
    /*  49 */ (void *)sys_notimp,
    /*  50 */ (void *)sys_getpid,
    /*  51 */ (void *)sys_getppid,
    /*  52 */ (void *)sys_exit,
    /*  53 */ (void *)sys_sched_yield,
    /*  54 */ (void *)sys_notimp,
    /*  55 */ (void *)sys_notimp,
    /*  56 */ (void *)sys_notimp,
    /*  57 */ (void *)sys_notimp,
    /*  58 */ (void *)sys_notimp,
    /*  59 */ (void *)sys_notimp,
    /*  60 */ (void *)sys_getuid,
    /*  61 */ (void *)sys_geteuid,
    /*  62 */ (void *)sys_setuid,
    /*  63 */ (void *)sys_seteuid,
    /*  64 */ (void *)sys_getgid,
    /*  65 */ (void *)sys_getegid,
    /*  66 */ (void *)sys_setgid,
    /*  67 */ (void *)sys_setegid,
    /*  68 */ (void *)sys_notimp,
    /*  69 */ (void *)sys_notimp,
    /*  70 */ (void *)sys_notimp,
    /*  71 */ (void *)sys_notimp,
    /*  72 */ (void *)sys_notimp,
    /*  73 */ (void *)sys_notimp,
    /*  74 */ (void *)sys_notimp,
    /*  75 */ (void *)sys_notimp,
    /*  76 */ (void *)sys_notimp,
    /*  77 */ (void *)sys_notimp,
    /*  78 */ (void *)sys_notimp,
    /*  79 */ (void *)sys_notimp,
    /*  80 */ (void *)sys_notimp,
    /*  81 */ (void *)sys_notimp,
    /*  82 */ (void *)sys_notimp,
    /*  83 */ (void *)sys_notimp,
    /*  84 */ (void *)sys_notimp,
    /*  85 */ (void *)sys_notimp,
    /*  86 */ (void *)sys_notimp,
    /*  87 */ (void *)sys_notimp,
    /*  88 */ (void *)sys_notimp,
    /*  89 */ (void *)sys_notimp,
    /*  90 */ (void *)sys_notimp,
    /*  91 */ (void *)sys_notimp,
    /*  92 */ (void *)sys_notimp,
    /*  93 */ (void *)sys_notimp,
    /*  94 */ (void *)sys_notimp,
    /*  95 */ (void *)sys_notimp,
    /*  96 */ (void *)sys_notimp,
    /*  97 */ (void *)sys_notimp,
    /*  98 */ (void *)sys_notimp,
    /*  99 */ (void *)sys_notimp,
    /* 100 */ (void *)sys_uname,
    /* 101 */ (void *)sys_notimp,
    /* 102 */ (void *)sys_notimp,
    /* 103 */ (void *)sys_notimp,
    /* 104 */ (void *)sys_notimp,
    /* 105 */ (void *)sys_notimp,
    /* 106 */ (void *)sys_notimp,
    /* 107 */ (void *)sys_notimp,
    /* 108 */ (void *)sys_notimp,
    /* 109 */ (void *)sys_notimp,
    /* 110 */ (void *)sys_notimp,
    /* 111 */ (void *)sys_notimp,
    /* 112 */ (void *)sys_notimp,
    /* 113 */ (void *)sys_notimp,
    /* 114 */ (void *)sys_notimp,
    /* 115 */ (void *)sys_notimp,
    /* 116 */ (void *)sys_notimp,
    /* 117 */ (void *)sys_notimp,
    /* 118 */ (void *)sys_notimp,
    /* 119 */ (void *)sys_notimp,
    /* 120 */ (void *)sys_notimp,
    /* 121 */ (void *)sys_notimp,
    /* 122 */ (void *)sys_notimp,
    /* 123 */ (void *)sys_notimp,
    /* 124 */ (void *)sys_notimp,
    /* 125 */ (void *)sys_notimp,
    /* 126 */ (void *)sys_notimp,
    /* 127 */ (void *)sys_notimp
};
