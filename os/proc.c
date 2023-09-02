/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <mosnix/attributes.h>
#include <mosnix/proc.h>
#include <mosnix/sched.h>
#include <mosnix/syscall.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

/** Process table for the kernel */
static struct proc process_table[CONFIG_PROC_MAX] ATTR_SECTION_NOINIT;

/** List of unused process blocks */
static struct run_queue unused_procs;

struct proc * volatile current_proc ATTR_SECTION_ZP;
uint8_t volatile in_kernel ATTR_SECTION_ZP;

void proc_init(void)
{
    struct proc *p;
    pid_small_t pid;
    STAILQ_INIT(&unused_procs);
    for (pid = 1, p = process_table; pid <= CONFIG_PROC_MAX; ++pid, ++p) {
        p->pid = pid;
        p->ppid = PID_UNUSED;
        p->state = PROC_UNUSED;
        STAILQ_INSERT_TAIL(&unused_procs, p, next);
    }
    current_proc = &(process_table[0]);
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
    int err;

    /* Find the next available unused process */
    p = STAILQ_FIRST(&unused_procs);
    if (!p)
        return -ENOMEM;
    STAILQ_REMOVE_HEAD(&unused_procs, next);

    /* Zero the entire process structure */
    memset(p, 0, sizeof(struct proc));
    p->ppid = ppid;

    /* Format the argc/argv arguments into the process structure */
    err = proc_format_args(p, argc, argv);
    if (err != 0)
        return err;

    /* Allocate zero page memory to the process and clear it */
    p->context.zp = (uint8_t *)((p->pid + 1) * PROC_ZP_SIZE);
    memset(p->context.zp, 0, PROC_ZP_SIZE);

    /* Process block is ready to go */
    *proc = p;
    return 0;
}

void proc_free(struct proc *proc)
{
    proc->state = PROC_UNUSED;
    STAILQ_INSERT_TAIL(&unused_procs, proc, next);
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

static inline void proc_push_stack_frame(struct proc *p, uint16_t value)
{
    uint8_t S = p->context.S - sizeof(struct proc_stack_frame);
    struct proc_stack_frame *frame =
        (struct proc_stack_frame *)(p->context.stack + S + 1);
    frame->A = (uint8_t)value;
    frame->X = (uint8_t)(value >> 8);
    p->context.S = S;
}

static inline void proc_set_arg2(struct proc *p, uint16_t value)
{
    p->context.zp[2] = (uint8_t)value;
    p->context.zp[3] = (uint8_t)(value >> 8);
}

static void proc_exit(int status)
{
    /* TODO */
    _exit(status);
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
    p->context.S = PROC_STACK_SIZE - 1;
    proc_push_return_stack(p, (uintptr_t)proc_exit);
    proc_push_return_stack(p, ((uintptr_t)func) + 1);
    proc_push_stack_frame(p, *((uint16_t *)(p->args)));    /* argc */
    proc_set_arg2(p, (uint16_t)(uintptr_t)(p->args + 2));  /* argv */

    /* Process is now runnable */
    sched_set_runnable(p);
    return p->pid;
}

/* Entry point to the in-ROM shell code, provided by the linker script. */
int shell_start(int argc, char **argv);

void proc_start_shell(void)
{
    static char * const shell_argv[] = {"/bin/sh", 0};
    proc_start_internal(0, shell_start, 1, (char **)shell_argv);
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
    /* TODO */
    (void)args;
}
