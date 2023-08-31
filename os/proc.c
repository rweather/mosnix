/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <mosnix/attributes.h>
#include <mosnix/proc.h>
#include <mosnix/syscall.h>
#include <string.h>
#include <errno.h>

/** Process table for the kernel */
static struct proc process_table[CONFIG_PROC_MAX] ATTR_SECTION_NOINIT;

struct proc * volatile current_proc = process_table;

void proc_init(void)
{
    struct proc *p;
    pid_small_t pid;
    for (pid = 0, p = process_table; pid < CONFIG_PROC_MAX; ++pid, ++p) {
        p->ppid = PID_UNUSED;
        p->state = PROC_UNUSED;
    }
}

static int proc_format_args(struct proc *proc, int argc, char **argv)
{
    struct arg_array *aa;
    char *data;
    size_t size;
    int arg;

    /* Validate the parameters */
    if (!argv)
        return -EFAULT;
    if (argc < 1)
        return -EINVAL;
    if (argc > CONFIG_ARGC_MAX)
        return -E2BIG;

    /* Prepare to convert the arguments into an in-place array in proc->args */
    struct arg_array {
        int argc;
        char *argv[CONFIG_ARGC_MAX + 1];
    };
    aa = (struct arg_array *)(proc->args);
    size = argc * 2 + 4;
    data = proc->args + size;
    size = CONFIG_ARG_MAX - size;

    /* Convert the arguments */
    aa->argc = argc;
    for (arg = 0; arg < argc; ++arg) {
        char *av = argv[arg];
        size_t len;
        if (!av)
            return -EFAULT;
        len = strlen(av);
        if (len >= size)
            return -E2BIG;
        aa->argv[arg] = data;
        memcpy(data, av, len);
        data[len] = '\0';
        data += len + 1;
        size -= len + 1;
    }
    aa->argv[arg] = 0;

    /* Arguments are ready to go */
    return 0;
}

int proc_create(pid_t ppid, int argc, char **argv, struct proc **proc)
{
    struct proc *p;
    pid_small_t pid;
    int err;
    for (pid = 0, p = process_table; pid < CONFIG_PROC_MAX; ++pid, ++p) {
        if (p->state == PROC_UNUSED) {
            memset(p, 0, sizeof(struct proc));
            p->ppid = ppid;
            err = proc_format_args(p, argc, argv);
            if (err != 0)
                return err;
            *proc = p;
            return 0;
        }
    }
    return -ENOMEM;
}

void proc_kill(struct proc *proc)
{
    /* TODO */
    proc->state = PROC_UNUSED;
}

static inline void proc_set_arg1(struct proc *p, uint16_t value)
{
    p->context.A = (uint8_t)value;
    p->context.X = (uint8_t)(value >> 8);
}

static inline void proc_set_arg2(struct proc *p, uint16_t value)
{
    p->context.zp[2] = (uint8_t)value;
    p->context.zp[3] = (uint8_t)(value >> 8);
}

static void proc_push_return_stack(struct proc *p, uintptr_t value)
{
    uint8_t S = p->context.S - 2;
    --value; /* Return addresses are -1 from the true address */
    p->context.stack[S]     = (uint8_t)value;
    p->context.stack[S + 1] = (uint8_t)(value >> 8);
    p->context.S = S;
}

static void proc_exit(int status)
{
    /* TODO */
    (void)status;
}

int proc_start_internal
    (pid_t ppid, proc_internal_func_t func, int argc, char **argv)
{
    struct proc *p;
    int err;

    /* Create the process control block */
    err = proc_create(ppid, argc, argv, &p);
    if (err != 0)
        return err;

    /* Configure the new process so that it will jump to "func"
     * when it starts executing.  If "func" returns, then arrange
     * to perform an "_exit" system call. */
    proc_set_arg1(p, *((uint16_t *)(p->args)));           /* argc */
    proc_set_arg2(p, (uint16_t)(uintptr_t)(p->args + 2)); /* argv */
    p->context.S = PROC_STACK_SIZE;
    proc_push_return_stack(p, (uintptr_t)proc_exit);
    proc_push_return_stack(p, (uintptr_t)func);

    /* Process is now runnable */
    p->state = PROC_RUNNING;
    return (p - process_table) + 1;
}

int sys_getpid(void)
{
    return (current_proc - process_table) + 1;
}

int sys_getppid(void)
{
    return current_proc->ppid;
}

void sys_exit(struct sys_exit_s *args)
{
    /* TODO */
    (void)args;
}
