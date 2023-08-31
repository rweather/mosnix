/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <mosnix/syscall.h>
#include <errno.h>

/* FIXME: At the moment there is only enough to make stdin/stdout/stderr work */

extern int getchar(void);
extern void __putchar(char c);

int sys_read(struct sys_read_s *args)
{
    /* TODO */
    if (args->fd == 0) {
        char *data = (char *)(args->data);
        size_t size = args->size;
        int result = 0;
        while (size > 0) {
            int c = getchar();
            if (c < 0)
                break;
            data[result++] = c;
            if (c == '\n')
                break; /* Terminate input at end of line like Unix tty's */
            --size;
        }
        return result;
    } else if (args->fd == 1 || args->fd == 2) {
        return -EINVAL;
    } else {
        return -EBADF;
    }
}

int sys_write(struct sys_write_s *args)
{
    /* TODO */
    if (args->fd == 1 || args->fd == 2) {
        const char *data = (const char *)(args->data);
        size_t size = args->size;
        int result = 0;
        while (size > 0) {
            __putchar(data[result++]);
            --size;
        }
        return result;
    } else if (args->fd == 0) {
        return -EINVAL;
    } else {
        return -EBADF;
    }
}

int sys_open(struct sys_open_s *args)
{
    /* TODO */
    (void)args;
    return -EPERM;
}

int sys_close(struct sys_close_s *args)
{
    /* TODO */
    (void)args;
    return -EPERM;
}

int sys_lseek(struct sys_lseek_s *args)
{
    /* TODO */
    (void)args;
    return -EPERM;
}
