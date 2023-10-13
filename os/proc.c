/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <mosnix/attributes.h>
#include <mosnix/proc.h>
#include <mosnix/file.h>
#include <mosnix/kmalloc.h>
#include <mosnix/printk.h>
#include <mosnix/sched.h>
#include <mosnix/syscall.h>
#include <bits/fcntl.h>
#include "drivers/tty/console.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>

/** Process table for the kernel */
static struct proc *process_table[CONFIG_PROC_MAX];

struct proc * volatile current_proc ATTR_SECTION_ZP;
uint8_t volatile in_kernel ATTR_SECTION_ZP;

void proc_init(void)
{
    current_proc = NULL;
}

int proc_create(pid_t ppid, int argc, char **argv, struct proc **proc)
{
    struct proc *p;
    pid_small_t pid;
    char **argv_copy;

    /* Validate the parameters */
    if (!argv)
        return -EFAULT;
    if (argc < 1)
        return -EINVAL;
    if (argc > CONFIG_ARGC_MAX)
        return -E2BIG;

    /* Allocate user space memory for the arguments first */
    argv_copy = kmalloc_copy_argv(argc, argv);
    if (!argv_copy) {
        return -ENOMEM;
    }

    /* Find the next available unused process */
    for (pid = 0; pid < CONFIG_PROC_MAX; ++pid) {
        if (process_table[pid] == NULL) {
            break;
        }
    }
    if (pid >= CONFIG_PROC_MAX) {
        kmalloc_user_free(argv_copy);
        return -ENOMEM;
    }

    /* Allocate user space memory for the process structure */
    p = kmalloc_user_alloc(sizeof(struct proc));
    if (!p) {
        kmalloc_user_free(argv_copy);
        return -ENOMEM;
    }
    process_table[pid] = p;

    /* Initialize the process structure */
    memset(p, 0, sizeof(struct proc));
    p->pid = pid + 1; /* Process identifiers are 1-based */
    p->ppid = ppid;
    p->argv = argv_copy;

    /* Allocate zero page memory to the process and clear it */
    p->zp = (uint8_t *)((pid + 2) * PROC_ZP_SIZE);
    memset(p->zp, 0, PROC_ZP_SIZE);

    /* Inherit properties from the parent, or set the defaults for pid 1 */
    if (ppid) {
        struct proc *parent = process_table[ppid - 1];
        memcpy(p->cwd, parent->cwd, sizeof(p->cwd));
        p->umask = parent->umask;
    } else {
        memcpy(p->cwd, "/root", 6);
        p->umask = S_IWGRP | S_IWOTH; /* 022 */
    }

    /* Process block is ready to go */
    *proc = p;
    return 0;
}

void proc_free(struct proc *proc)
{
    process_table[proc->pid - 1] = NULL;
    kmalloc_user_free(proc->argv);
    proc->state = PROC_UNUSED;
    kmalloc_user_free(proc);
    /* TODO */
}

static void proc_push_return_stack(struct proc *p, uintptr_t value)
{
    uint8_t S = p->context.S - 2;
    /* Return address is the value minus 1 */
    --value;
    /* S points to one below the top of stack, not the top of stack */
    p->context.stack[S + 1] = (uint8_t)value;
    p->context.stack[S + 2] = (uint8_t)(value >> 8);
    p->context.S = S;
}

static inline void proc_push_byte(struct proc *p, uint8_t value)
{
    uint8_t S = p->context.S;
    p->context.stack[S] = (uint8_t)value;
    p->context.S = S - 1;
}

static inline void proc_set_arg2(struct proc *p, uint16_t value)
{
    p->zp[2] = (uint8_t)value;
    p->zp[3] = (uint8_t)(value >> 8);
}

void proc_stop(int status)
{
    /* TODO */
    _exit(status);
}

int proc_create_internal
    (pid_t ppid, proc_internal_func_t func, int argc, char **argv,
     struct proc **proc)
{
    struct proc *p;
    int err;

    /* Create the process control block */
    err = proc_create(ppid, argc, argv, proc);
    if (err != 0)
        return err;
    p = *proc;

    /* Configure the new process so that it will jump to "func"
     * when it starts executing.  If "func" returns, then arrange
     * to perform an "_exit" system call. */
    p->context.S = CONFIG_RETURN_STACK_SIZE - 1;
    proc_push_return_stack(p, (uintptr_t)proc_stop);
    proc_push_return_stack(p, ((uintptr_t)func) + 1);
    proc_push_byte(p, 0x00); /* P flags to pass to the new process */
    p->context.AX = argc;
    proc_set_arg2(p, (uint16_t)(uintptr_t)(p->argv));

    /* Process is now runnable */
    sched_set_runnable(p);
    return p->pid;
}

/* Entry point to the in-ROM shell code, provided by the linker script. */
int shell_start(int argc, char **argv);

void proc_start_shell(void)
{
    static char * const shell_argv[] = {"/bin/sh", 0};
    struct proc *p = 0;
    struct file *console;

    /* Create the shell process; assumed to always succeed */
    proc_create_internal(0, shell_start, 1, (char **)shell_argv, &p);

    /* Set up the console tty as stdin, stdout, and stderr for the shell */
    console = file_new(O_RDWR, 0660);
    open_console_tty(console);
    p->fd[0] = console;
    file_ref(console);
    p->fd[1] = console;
    file_ref(console);
    p->fd[2] = console;
}

int sys_getpid(void)
{
    return current_proc->pid;
}

int sys_getppid(void)
{
    return current_proc->ppid;
}

void sys_exit(struct sys_exit_s *args)
{
    proc_stop(args->status);
}
