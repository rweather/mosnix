/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <mosnix/sem.h>
#include <mosnix/sched.h>
#include <mosnix/proc.h>
#include <errno.h>

/*
 * We rely on the "in_kernel" variable being set to non-zero to block
 * pre-emption while we are operating on semaphores.  Without this,
 * the semaphore operations would not act in an atomic manner.
 *
 * The one exception is sem_signal_isr() which may be called from
 * either inside or outside the kernel context.
 */

void sem_init(struct sem *sem, sem_value_t init_value)
{
    sem->value = init_value;
    TAILQ_INIT(&(sem->waiters));
}

static void sem_wakeup(struct sem *sem)
{
    struct proc *p = TAILQ_FIRST(&(sem->waiters));
    if (p) {
        TAILQ_REMOVE(&(sem->waiters), p, qptrs);
        p->context.AX = 0;
        p->wait_sem = NULL;
        sched_set_runnable(p);
    } else {
        sem->value = 1;
    }
}

void sem_signal(struct sem *sem)
{
    sem_value_t value = sem->value;
    if (!value)
        sem_wakeup(sem);
    else
        sem->value = value + 1;
}

void sem_signal_monitor(struct sem *sem)
{
    if (!(sem->value))
        sem_wakeup(sem);
}

void sem_signal_isr(struct sem *sem)
{
    /* TODO */
    (void)sem;
}

int sem_wait(struct sem *sem)
{
    sem_value_t value = sem->value;
    if (value) {
        /* We were able to acquire the semaphore immediately */
        sem->value = value - 1;
        return 0;
    } else {
        /* Semaphore value is zero, so add this process to the waiting list */
        struct proc *p = current_proc;
        sched_remove_runnable(p);
        TAILQ_INSERT_TAIL(&(sem->waiters), p, qptrs);
        p->wait_sem = sem;
        p->state = PROC_SLEEP;

        /* Ask the scheduler to run something else until it is time to
         * wake this process up again.  When we are woken, the return
         * value of schedule() will be 0, -EBUSY, or -EINTR for the
         * result of the wait operation. */
        return schedule();
    }
}

int sem_wait_timed(struct sem *sem, clock_t timeout)
{
    /* TODO: handle timeouts */
    (void)timeout;
    return sem_wait(sem);
}

void sem_interrupt(struct sem *sem)
{
    for (;;) {
        struct proc *p = TAILQ_FIRST(&(sem->waiters));
        if (p) {
            TAILQ_REMOVE(&(sem->waiters), p, qptrs);
            p->context.AX = -EINTR;
            p->wait_sem = NULL;
            sched_set_runnable(p);
        } else {
            break;
        }
    }
}
