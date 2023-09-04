/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <mosnix/sched.h>
#include <mosnix/syscall.h>
#include <mosnix/printk.h>
#include <errno.h>
#include <stdlib.h>

/**
 * @brief List of all runnable processes in the system, except for the
 * one that is currently running.
 */
static struct run_queue runnable = TAILQ_HEAD_INITIALIZER(runnable);

/**
 * @brief Switches to a different process and continues running it.
 *
 * @param[in] proc The process to switch to.  Must not be NULL
 * although the previous value of "current_proc" can be NULL.
 *
 * @return The status or error code that was passed from the current
 * process to @a proc.
 *
 * This function will return when the current process can continue
 * from where it left off.
 */
__attribute__((leaf)) int proc_switch_to(struct proc *proc);

void sched_set_runnable(struct proc *proc)
{
    if (proc->state != PROC_RUNNING) {
        proc->state = PROC_RUNNING;
        TAILQ_INSERT_TAIL(&runnable, proc, qptrs);
    }
}

void sched_remove_runnable(struct proc *proc)
{
    if (proc->state == PROC_RUNNING) {
        proc->state = PROC_NOT_RUNNING;
        TAILQ_REMOVE(&runnable, proc, qptrs);
    }
}

int schedule(void)
{
    /* TODO: This is very basic and probably not what we want */

    /* Find a runnable process and schedule it */
    struct proc *proc = TAILQ_FIRST(&runnable);
    if (!proc) {
        /* Nothing is runnable, so the system is dead! */
        kputstr("No runnable processes found - halting!\n");
        _exit(1);
    }
    proc_switch_to(proc);
    return 0;
}

int sys_sched_yield(void)
{
    /* TODO */
    return 0;
}
