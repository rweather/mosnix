/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#ifndef MOSNIX_SEM_H
#define MOSNIX_SEM_H

#include <sys/types.h>
#include <sys/queue.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Declare the struct run_queue type */
TAILQ_HEAD(run_queue, proc);

/**
 * @brief Type for the value of a semaphore.
 */
typedef u_char sem_value_t;

/**
 * @brief Semaphore primitive.
 */
struct sem
{
    /** Value of the semaphore */
    sem_value_t value;

    /** List of processes that are currently waiting on the semaphore. */
    struct run_queue waiters;
};

/**
 * @brief Initializes a semaphore.
 *
 * @param[out] sem The semaphore to initialize.
 * @param[in] init_value The initial value of the semaphore.
 */
void sem_init(struct sem *sem, sem_value_t init_value);

/**
 * @brief Signals a semaphore by incrementing its value.
 *
 * @param[in,out] sem The semaphore to signal.
 *
 * If there are waiting processes, then the first on the list will be woken.
 */
void sem_signal(struct sem *sem);

/**
 * @brief Signals a semaphore in "monitor" mode.
 *
 * @param[in,out] sem The semaphore to signal.
 *
 * This function increments the semaphore if it is zero and wakes a single
 * waiting process.  If the semaphore is non-zero, then the semaphore will
 * not be incremented.
 *
 * This can be used to safely signal a waiting process if there is one,
 * but do nothing if there are no waiting processes.  The initial value
 * of the semaphore must be 1 for mutual exclusion.
 */
void sem_signal_monitor(struct sem *sem);

/**
 * @brief Signals a semaphore by incrementing its value (ISR safe).
 *
 * @param[in,out] sem The semaphore to signal.
 *
 * The semaphore must be operating in "monitor" mode.  If there are waiting
 * processes, then the first on the list will be woken.  If there are no
 * waiting processes, then the value will not be incremented.
 *
 * This is the only function in this API that can be called from an
 * interruption service routine (ISR) context.
 */
__attribute__((interrupt, no_isr)) void sem_signal_isr(struct sem *sem);

/**
 * @brief Waits for a semaphore to become available and decrements it.
 *
 * @param[in,out] sem The semaphore to wait on.
 *
 * @return 0 if the semaphore was acquired, or -EINTR if the operation
 * was interrupted.
 */
int sem_wait(struct sem *sem);

/**
 * @brief Waits for a semaphore to become available and decrements it,
 * or give up after a timeout.
 *
 * @param[in,out] sem The semaphore to wait on.
 * @param[in] timeout Timeout in clock ticks.
 *
 * @return 0 if the semaphore was acquired, -EBUSY on timeout,
 * or -EINTR if the operation was interrupted.
 */
int sem_wait_timed(struct sem *sem, clock_t timeout);

/**
 * @brief Interrupts all processes that are waiting on a semaphore.
 *
 * @param[in,out] sem The semaphore to interrupt.
 *
 * The value of the semaphore will not be changed.
 */
void sem_interrupt(struct sem *sem);

#ifdef __cplusplus
}
#endif

#endif
