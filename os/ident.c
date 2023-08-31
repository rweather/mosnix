/*
 * Copyright (C) 2023 Southern Storm Software, Pty Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <mosnix/syscall.h>
#include <errno.h>

/* For now, everyone is root.  Please Unix responsibly! */

static id_t sys_getid(void)
{
    return 0;
}

static int sys_setid(id_t uid)
{
    if (id != 0)
        return -EINVAL;
    else
        return 0;
}

uid_t sys_getuid(void)
{
    return sys_getid();
}

uid_t sys_geteuid(void)
{
    return sys_getid();
}

int sys_setuid(uid_t uid)
{
    return sys_setid(uid);
}

int sys_seteuid(uid_t uid)
{
    return sys_setid(uid);
}

gid_t sys_getgid(void)
{
    return sys_getid();
}

gid_t sys_getegid(void)
{
    return sys_getid();
}

int sys_setgid(gid_t gid)
{
    return sys_setid(gid);
}

int sys_setegid(gid_t gid)
{
    return sys_setid(gid);
}
