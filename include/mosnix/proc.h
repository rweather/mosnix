/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#ifndef MOSNIX_PROC_H
#define MOSNIX_PROC_H

#include <mosnix/attributes.h>
#include <mosnix/config.h>
#include <sys/types.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @typedef pid_small_t
 * @brief Small version of pid_t which is large enough to hold process
 * identifiers between 0 and CONFIG_PROC_MAX.
 *
 * CONFIG_PROC_MAX is used to indicate an unused process structure.
 */
#if CONFIG_PROC_MAX < 255
typedef uint8_t pid_small_t;
#else
typedef pid_t pid_small_t;
#endif

/**
 * @brief Special process identifier for "unused".
 */
#define PID_UNUSED 0

/**
 * @brief Process states.
 *
 * Note that not all of these may be in use at the moment.
 */
enum ATTR_ENUM_PACKED proc_state
{
    /** Process identifier is not currently in use */
    PROC_UNUSED,

    /** Process is running */
    PROC_RUNNING,

    /** Process is stopped due to job control */
    PROC_STOPPED_JOB_CONTROL,

    /** Process is stopped due to debug tracing */
    PROC_STOPPED_DEBUGGER,

    /** Interruptible sleep, waiting on a semaphore */
    PROC_SLEEP,

    /** Uninterruptible sleep, waiting on a semaphore */
    PROC_UNINT_SLEEP,

    /** Process is a zombie, waiting for its parent to reap it */
    PROC_ZOMBIE
};

/**
 * @brief Maximum number of zero page locations that each process can use.
 */
#define PROC_ZP_SIZE 32

/**
 * @brief Maximum amount of stack space the process can use between
 * 0x0100 and 0x1FF.
 *
 * This is also the maximum "S" value that the process may use.
 */
#define PROC_STACK_SIZE 64

/**
 * @brief Saved process state to support context switching.
 */
struct proc_context
{
    /** Accumulator */
    uint8_t A;

    /** X index register */
    uint8_t X;

    /** Y index register */
    uint8_t Y;

    /** Processor status register */
    uint8_t P;

    /** Stack pointer */
    uint8_t S;

    /** Address in the zero page to where zp has been "swapped in".
     *  Set to zero if zp is currently "swapped out". */
    uint8_t zp_in;

    /** Saved zero page values when they are "swapped out" */
    uint8_t zp[PROC_ZP_SIZE];

    /** Saved locations from the 6502 stack */
    uint8_t stack[PROC_STACK_SIZE];
};

/**
 * @brief Information about a process in the kernel.
 */
struct proc
{
    /** Parent process identifier, or PID_UNUSED if this process
     *  does not have a parent. */
    pid_small_t ppid;

    /** Current process state */
    enum proc_state state;

    /** Saved context information for the process */
    struct proc_context context;

    /** Arguments to the process, formatted as an array of strings. */
    char args[CONFIG_ARG_MAX];
};

/**
 * @brief Entry point for a user process that is implemented inside the kernel.
 *
 * @param[in] argc Number of arguments.
 * @param[in] argv Array of arguments to pass to the process.
 *
 * @return Exit status for the process when it ends.
 */
typedef int (*proc_internal_func_t)(int argc, char **argv);

/**
 * @brief Reference to the current process that is running.
 */
extern struct proc * volatile current_proc;

/**
 * @brief Initializes the process subsystem of the kernel.
 */
void proc_init(void);

/**
 * @brief Creates a process and initializes the command-line arguments.
 *
 * @param[in[ ppid Identifier for the parent process.
 * @param[in] argc Number of arguments, including the program name.
 * @param[in] argv Points to the argument array, terminated by a NUL.
 * @param[out] proc Returns a pointer to the new process blockk on success.
 *
 * @return Zero on success or an error code.
 */
int proc_create(pid_t ppid, int argc, char **argv, struct proc **proc);

/**
 * @brief Kills a process.
 *
 * @param[in] proc Points to the process to kill.
 */
void proc_kill(struct proc *proc);

/**
 * @brief Starts a process that is implemented inside the kernel itself.
 *
 * @param[in[ ppid Identifier for the parent process.
 * @param[in] func Points to the function to invoke for the internal process.
 * @param[in] argc Number of arguments, including the program name.
 * @param[in] argv Points to the argument array, terminated by a NUL.
 *
 * @return The process identifier for the new process or an error code.
 */
int proc_start_internal
    (pid_t ppid, proc_internal_func_t func, int argc, char **argv);

#ifdef __cplusplus
}
#endif

#endif
