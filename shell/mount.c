/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include "command.h"
#include "fstab.h"
#include "print.h"
#include <mosnix/config.h>

#define MOUNT_MAX 2

struct fstab_entry
{
    const char *mountpoint;
    const char *device;
    const char *type;
    const char *options;
};
static struct fstab_entry mounts[MOUNT_MAX];
static unsigned char mount_count = 0;

void add_fstab
    (const char *mountpoint, const char *device, const char *type,
     const char *options)
{
    if (mount_count < MOUNT_MAX) {
        struct fstab_entry *entry = &(mounts[mount_count++]);
        entry->mountpoint = mountpoint;
        entry->device = device;
        entry->type = type;
        entry->options = options;
    }
}

int cmd_mount(int argc, char **argv)
{
    /* At the moment, we can only display the mount points, not change them */
    (void)argc;
    (void)argv;
    unsigned char index;
    for (index = 0; index < mount_count; ++index) {
        struct fstab_entry *entry = &(mounts[index]);
        print_string(entry->device);
        print_string(" on ");
        print_string(entry->mountpoint);
        print_string(" type ");
        print_string(entry->type);
        print_string(" (");
        print_string(entry->options);
        print_string(")\n");
    }
    return 0;
}
