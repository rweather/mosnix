/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <getopt.h>
#include <string.h>
#include <unistd.h>

char *optarg = 0;
int optind = 0;
static int nextind = 1;
static char *nextchar = 0;

static void unknown_option(const char *name, char chname)
{
    write(2, "unknown option `", 16);
    if (chname) {
        char buf[2];
        buf[0] = '-';
        buf[1] = chname;
        write(2, buf, 2);
    } else {
        write(2, name, strlen(name));
    }
    write(2, "'\n", 2);
}

static void missing_argument(char chname)
{
    write(2, "missing argument for `-", 22);
    write(2, &chname, 1);
    write(2, "'\n", 2);
}

int getopt(int argc, char * const argv[], const char *options)
{
    char ch;
    const char *opt;

    /* Check for forced reset of the option parsing */
    if (optind == 0) {
        optind = 1;
        nextind = 1;
        nextchar = 0;
    }
    optarg = 0;

    /* Do we have more characters to parse from the previous argument? */
    if (!nextchar) {
        /* No, then see if the next argument is an option */
        if (nextind >= argc || argv[nextind] == 0) {
            optind = nextind++;
            return -1;
        }
        if (argv[nextind][0] == '-' && argv[nextind][1] == '-') {
            /* Long option starting with "--".  We don't support long
             * options, except for "--" to terminate the main options. */
            if (argv[nextind][2] != '\0') {
                unknown_option(argv[nextind], '\0');
                optind = nextind;
                return '?';
            }
            ++nextind;
            optind = nextind;
            return -1;
        } else if (argv[nextind][0] == '-' && argv[nextind][1] != '\0') {
            /* Start of option characters */
            nextchar = argv[nextind] + 1;
            optind = nextind;
        } else {
            /* Regular name or "-" on its own terminates the options */
            optind = nextind;
            return -1;
        }
    }

    /* Process the next option character */
    ch = *nextchar++;
    opt = strchr(options, ch);
    if (!opt) {
        unknown_option(0, ch);
        optind = nextind;
        return '?';
    }
    if (opt[1] == ':') {
        /* This option expects an argument */
        optind = nextind++;
        if (*nextchar != '\0') {
            optarg = nextchar;
            nextchar = 0;
        } else if (nextind < argc) {
            optarg = argv[nextind++];
            optind = nextind;
        } else {
            missing_argument(ch);
            optind = nextind++;
            return '?';
        }
    } else {
        /* Flag option on its own */
        optind = nextind;
        if (*nextchar == '\0') {
            nextchar = 0;
            ++nextind;
        }
    }
    return ch;
}
