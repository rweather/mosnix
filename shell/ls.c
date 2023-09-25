/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include "command.h"
#include <dirent.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <errno.h>

#define LS_COLUMN       0x01
#define LS_LONG         0x02
#define LS_HIDDEN       0x04
#define LS_CLASSIFY     0x08
#define LS_NAMED        0x10
#define LS_NEXT         0x20

static int list(const char *spec, unsigned char options);

int cmd_ls(int argc, char **argv)
{
    unsigned char options = 0;
    int opt, exitval;

    /* Parse the command-line options */
    while ((opt = getopt(argc, argv, "ClaF")) >= 0) {
        switch (opt) {
        case 'C':   options |= LS_COLUMN; break;
        case 'l':   options |= LS_LONG; break;
        case 'a':   options |= LS_HIDDEN; break;
        case 'F':   options |= LS_CLASSIFY; break;
        default: return 1;
        }
    }
    if (!(options & (LS_COLUMN | LS_LONG))) {
        /* Default to column mode */
        options |= LS_COLUMN;
    }

    /* List files for all directories / wildcards on the command-line */
    exitval = 0;
    if (optind >= argc) {
        if (!list(".", options)) {
            exitval = 1;
        }
    } else {
        if ((optind + 1) < argc) {
            /* Name the directories as we list them */
            options |= LS_NAMED;
        }
        while (optind < argc) {
            if (!list(argv[optind], options)) {
                exitval = 1;
            }
            options |= LS_NEXT;
            ++optind;
        }
    }
    return exitval;
}

static void print_mode(u_char type, mode_t mode)
{
    static const char types[] = "?fc?d?b?-?l?s???";
    static const char bits[] = "rwxrwxrwx";
    const char *bitp = bits;
    mode_t bit;
    print_char(types[type]);
    for (bit = (1 << 8); bit != 0; bit >>= 1, ++bitp) {
        if (mode & bit) {
            print_char(*bitp);
        } else {
            print_char('-');
        }
    }
}

static int list(const char *spec, unsigned char options)
{
    DIR *dir = opendir(spec);
    struct dirent *entry;
    if (!dir) {
        print_error(spec);
        return 0;
    }
    if (options & LS_NAMED) {
        if (options & LS_NEXT)
            print_nl();
        print_string(spec);
        print_string(":\n");
    }
    /* TODO: sort the entries in the directory on name */
    errno = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.' && (options & LS_HIDDEN) == 0) {
            continue;
        }
        if (options & LS_LONG) {
            print_mode(entry->d_type, entry->d_mode_np);
            if (entry->d_type == DT_CHR || entry->d_type == DT_BLK) {
                dev_t dev = (dev_t)(entry->d_ino);
                print_number(major(dev), 6);
                print_char(',');
                print_number(minor(dev), 4);
            } else {
                print_number(entry->d_ino, 11);
            }
            /* TODO: print the file modification time */
            print_char(' ');
        }
        print_string(entry->d_name);
        if (options & LS_CLASSIFY) {
            if (entry->d_type == DT_DIR) {
                print_char('/');
            } else if (entry->d_type == DT_LNK) {
                print_char('@');
            } else if (entry->d_type == DT_REG) {
                /* Detect executable regular files */
                if (entry->d_mode_np & (S_IXUSR | S_IXGRP | S_IXOTH)) {
                    print_char('*');
                }
            }
        }
        print_nl();
        errno = 0;
    }
    if (errno != 0) {
        /* An error occurred during readdir() */
        print_error(spec);
    }
    closedir(dir);
    return 1;
}
