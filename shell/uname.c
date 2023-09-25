/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include "command.h"
#include <sys/utsname.h>

#define UNAME_SYSNAME  0x01
#define UNAME_NODENAME 0x02
#define UNAME_RELEASE  0x04
#define UNAME_VERSION  0x08
#define UNAME_MACHINE  0x10
#define UNAME_ALL      0xFF

#define print_uname_string(str) \
    do { \
        if (need_space) \
            print_char(' '); \
        else \
            need_space = 1; \
        print_string((str)); \
    } while (0)

int cmd_uname(int argc, char **argv)
{
    struct utsname uts;
    unsigned char need_space = 0;
    unsigned char options = 0;
    int opt;

    if (argc == 0) {
        /* Printing the startup banner for the system */
        options = UNAME_SYSNAME | UNAME_RELEASE | UNAME_MACHINE;
    } else {
        while ((opt = getopt(argc, argv, "asnrvm")) >= 0) {
            switch (opt) {
            case 'a':   options |= UNAME_ALL; break;
            case 's':   options |= UNAME_SYSNAME; break;
            case 'n':   options |= UNAME_NODENAME; break;
            case 'r':   options |= UNAME_RELEASE; break;
            case 'v':   options |= UNAME_VERSION; break;
            case 'm':   options |= UNAME_MACHINE; break;
            default: return 1;
            }
        }
        if (!options) {
            options = UNAME_SYSNAME;
        }
    }

    if (uname(&uts) < 0) {
        print_error("uname");
        return 1;
    }

    if (options & UNAME_SYSNAME)
        print_uname_string(uts.sysname);
    if (options & UNAME_NODENAME)
        print_uname_string(uts.nodename);
    if (options & UNAME_RELEASE)
        print_uname_string(uts.release);
    if (options & UNAME_VERSION)
        print_uname_string(uts.version);
    if (options & UNAME_MACHINE)
        print_uname_string(uts.machine);
    print_nl();

    return 0;
}
