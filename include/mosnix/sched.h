/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#ifndef MOSNIX_SCHED_H
#define MOSNIX_SCHED_H

#include <mosnix/proc.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the scheduler.
 */
void sched_init(void);

/**
 * @brief Start the scheduler running.  This function never returns.
 */
void sched_start(void);

/**
 * @brief Sets a process to runnable and adds it to the run queue.
 *
 * @param[in] proc The process.
 */
void sched_set_runnable(struct proc *proc);

/**
 * @brief Removes the current process from the run queue.
 *
 * @param[in] proc The process.
 */
void sched_remove_runnable(struct proc *proc);

/**
 * @brief Schedule the next process to run.
 *
 * @return The return status that was passed to this process when it
 * was woken up from sleep.  If the current process dies, then this
 * function will never return.
 */
int schedule(void);

#ifdef __cplusplus
}
#endif

#endif
