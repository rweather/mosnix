/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <mosnix/sched.h>
#include <mosnix/syscall.h>
#include <errno.h>
#include <stdlib.h>

/**
 * @brief List of all runnable processes in the system, except for the
 * one that is currently running.
 */
static struct run_queue runnable = STAILQ_HEAD_INITIALIZER(runnable);

/**
 * @brief Swaps out the current user process in preparation to switch
 * to a different process.
 *
 * The contents of the stack in the $0100 page are copied to the process block.
 *
 * On entry, "current_proc" must point to the current process.
 * On exit, "current_proc" will be set to NULL.
 */
__attribute__((leaf)) void swap_out(void);

/**
 * @brief Swaps in a process and starts running it.
 *
 * @param[in] proc The process to switch to.
 *
 * The contents of the process block are copied to the $0100 page if
 * @a proc is not the same as "current_proc".  Otherwise it is assumed
 * that the stack has not been swapped out.
 *
 * This function will not return to the caller.  The kernel will be
 * re-entered via a different path when the next system call occurs.
 */
__attribute__((leaf)) void swap_in(struct proc *proc);

void sched_set_runnable(struct proc *proc)
{
    if (proc != current_proc) {
        proc->state = PROC_RUNNING;
        STAILQ_INSERT_TAIL(&runnable, proc, next);
    }
}

void schedule(void)
{
    /* TODO: This is very basic and probably not what we want */

    /* Find a runnable process and schedule it */
    struct proc *proc = STAILQ_FIRST(&runnable);
    if (!proc) {
        /* Nothing is runnable, so the system is dead! */
        _exit(1);
    }
    swap_in(proc);
}

int sys_sched_yield(void)
{
    /* TODO */
    return 0;
}
