/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include <mosnix/proc.h>
#include <errno.h>

/**
 * @brief Swaps out the stack for the current user process.
 *
 * The contents of the stack in the $0100 page are copied to the process block.
 */
extern void swap_out_stack(void);

/**
 * @brief Swaps in the stack from the current user process.
 *
 * The contents of the process block are copied to the $0100 page.
 */
extern void swap_in_stack(void);

/**
 * @brief Dispatches a system call to the relevant handler.
 *
 * @param[in] arg Points to the arguments to the system call.
 * @param[in] handler Points to the handler function from the dispatch table.
 */
void dispatch_syscall(void *arg, int (*handler)(void *arg))
{
    /* Invoke the system call and put the result back into the stack frame */
    int result = (*handler)(arg);
    struct proc_stack_frame *frame =
        (struct proc_stack_frame *)(0x0100 + current_proc->context.S);
#if defined(CPU_65C02)
    frame->AX = (uint16_t)result;
#else
    frame->A = (uint8_t)result;
    frame->X = (uint8_t)(result >> 8);
#endif

    /* Find the next runnable process and switch to it */
    /* TODO */
}

void schedule(void)
{
    /* Find a runnable process and schedule it */
    /* TODO */
}
