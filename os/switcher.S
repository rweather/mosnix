;
; Copyright (c) 2023 Rhys Weatherley
;
; Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
; See https://github.com/rweater/mosnix/blob/main/LICENSE for license
; information.
;

;
; This is the core context-switching engine for MOSnix.
;
; Once the kernel has finished its initialization, it will arrange to
; run the shell as process 1 and then it calls run_scheduler().
; The context switcher constantly looks for runnable processes and
; dispatches them.
;
; When a process performs a system call, control goes to the SYSCALL()
; function which runs at the same kernel stack level as run_scheduler().
;

#include "imag.inc"

#define CPU_STACK 0x0100
#define PROC_STACK_SIZE 64

.global run_scheduler
.section .text.run_scheduler,"ax",@progbits
run_scheduler:
  tsx                       ; Save the kernel stack pointers.
  stx __kernel_stack
  lda __rc0
  sta __kernel_stack+1
  lda __rc1
  sta __kernel_stack+2
.Lrun_scheduler_loop:
  jsr schedule
  jmp .Lrun_scheduler_loop

;
; System call dispatcher.  This is called from the IRQBRK handler
; whenever we encounter a BRK instruction.  BRK is our system call trap.
;
.global brk_syscall
.section .text.brk_syscall,"axR",@progbits
brk_syscall:
;
; We are now in kernel space.  Block preemption.
;
  inc in_kernel

;
; Save the user stack pointer to the "current_proc" variable.
;
  ldy #0
  tsx
  txa
  sta (current_proc),y

;
; Load the kernel stack pointers from the values saved in run_scheduler().
;
  ldx __kernel_stack
  txs
  lda __kernel_stack+1
  sta __rc0
  lda __kernel_stack+2
  sta __rc1

;
; Dispatch the system call.
;
  lda (current_proc),y
  tay
#if defined(CPU_65C02)
  ldx CPU_STACK+2,y
#else
  ldx CPU_STACK,y
#endif
  lda SYSCALL_TABLE,x
  sta __rc2
  lda SYSCALL_TABLE+1,x
  sta __rc3
#if defined(CPU_65C02)
  lda CPU_STACK,y
  ldx CPU_STACK+1,y
#else
  lda CPU_STACK+2,y
  ldx CPU_STACK+2,y
#endif
  jsr dispatch_syscall

;
; Restore the user stack pointer from the "current_proc" variable.
;
  ldy #0
  lda (current_proc),y
  tax
  txs

;
; Now leaving kernel space.  Re-enable preemption.
;
  dec in_kernel

;
; Restore the registers from the user stack and return to user space.
;
#if defined(CPU_65C02)
  ply
  plx
  pla
#else /* !CPU_65C02 */
  pla
  tay
  pla
  tax
  pla
#endif /* !CPU_65C02 */
  rti

;
; Swap out the user stack for the current process.
;
; Everything from S up to PROC_STACK_SIZE is copied out of the
; 6502 return stack at $0100 to the process block.
;
.global swap_out_stack
.section .text.swap_out_stack,"ax",@progbits
swap_out_stack:
  ldy #0
  lda (current_proc),y
  tax
  clc
  adc #3
  tay
  inx ; S points to 1 below the top of stack, so adjust for that.
.Lswap_out_loop:
  cpx #PROC_STACK_SIZE
  bcs .Lswap_out_done
  lda CPU_STACK,x
  sta (current_proc),y
  iny
  inx
  bne .Lswap_out_loop
.Lswap_out_done:
  rts

;
; Swap in the user stack for the current process.
;
; Everything from S up to PROC_STACK_SIZE is copied out of the
; process block to the 6502 return stack at $0100.
;
.global swap_in_stack
.section .text.swap_in_stack,"ax",@progbits
swap_in_stack:
  ldy #0
  lda (current_proc),y
  tax
  clc
  adc #3
  tay
  inx ; S points to 1 below the top of stack, so adjust for that.
.Lswap_in_loop:
  cpx #PROC_STACK_SIZE
  bcs .Lswap_in_done
  lda (current_proc),y
  sta CPU_STACK,x
  iny
  inx
  bne .Lswap_in_loop
.Lswap_in_done:
  rts

.section .bss,"aw",@nobits
__kernel_stack:
  .fill 3
