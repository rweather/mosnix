/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <mosnix/file.h>
#include <mosnix/target.h>
#include <errno.h>

/* Basic TTY that wraps the target's __chrin() and __chrout() functions */

#if defined(CONFIG_CONSOLE_BASIC_TTY)

static ssize_t basic_tty_read(struct file *file, void *data, size_t size)
{
    /* TODO: Implement proper tty line disciplines */
    char *d = (char *)data;
    int result = 0;
    (void)file;
    while (size > 0) {
        int c = __chrin();
        if (c < 0)
            break;
        if (c == '\r' || c == '\n') {
            d[result++] = '\n';
#if !CONFIG_CHRIN_ECHO
#if CONFIG_CHROUT_LF_HANDLING
            __chrout('\n');
#else
            __chrout('\r');
            __chrout('\n');
#endif
#endif
            break;
        }
        d[result++] = c;
#if !CONFIG_CHRIN_ECHO
        if (c == '\b' || c == 0x7F) {
            __chrout('\b');
            __chrout(' ');
            __chrout('\b');
        } else {
            __chrout(c);
        }
#endif
        --size;
    }
    return result;
}

static ssize_t basic_tty_write(struct file *file, const void *data, size_t size)
{
    /* TODO: Implement proper tty line disciplines */
    const char *d = (const char *)data;
    int result = 0;
    (void)file;
    while (size > 0) {
        char c = *d++;
#if CONFIG_CHROUT_LF_HANDLING
        __chrout(c);
#else
        if (c != '\n') {
            __chrout(c);
        } else {
            __chrout('\r');
            __chrout('\n');
        }
#endif
        ++result;
        --size;
    }
    return result;
}

static struct file_operations const basic_tty_operations = {
    .close = file_close_default,
    .read = basic_tty_read,
    .write = basic_tty_write,
    .lseek = file_lseek_default,
    .ioctl = file_ioctl_default
};

int open_console_tty(struct file *file)
{
    file->op = &basic_tty_operations;
    return 0;
}

#endif /* CONFIG_CONSOLE_BASIC_TTY */
