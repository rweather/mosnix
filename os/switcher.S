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
; run the shell as process 1 and then it calls sched_start().  This function
; constantly looks for runnable processes and dispatches them.
;
; When a process performs a system call, control goes to the SYSCALL()
; function which runs at the same kernel stack level as sched_start().
;

#include "imag.inc"

#define CPU_STACK 0x0100
#define PROC_STACK_SIZE 64

.global sched_start
.section .text.sched_start,"ax",@progbits
sched_start:
  tsx                       ; Save the kernel stack pointers.
  stx __kernel_stack
  lda __rc0
  sta __kernel_stack+1
  lda __rc1
  sta __kernel_stack+2
  inc in_kernel             ; We are currently in the kernel context.
.Lrun_scheduler_loop:
  jsr schedule
  jmp .Lrun_scheduler_loop

;
; System call dispatcher.  This is called from the IRQBRK handler
; whenever we encounter a BRK instruction.  BRK is our system call trap.
;
; On entry to this function, the stack should be set up as follows
; (top of stack is to the left):
;
;   65c02:  X A P <return-address>
;   6502:   Y X A P <return-address>
;
; Y will be destroyed by this function.  A:X returns the system call result.
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
  tsx
  txa
#if defined(CPU_65C02)
  sta (current_proc)
#else
  ldy #0
  sta (current_proc),y
#endif

;
; Load the kernel stack pointers from the values saved in sched_start().
;
  ldx __kernel_stack
  txs
  ldx __kernel_stack+1
  stx __rc0
  ldx __kernel_stack+2
  stx __rc1

;
; Dispatch the system call.
;
#if defined(CPU_65C02)
  tax
  lda SYSCALL_TABLE,y
  sta __rc4
  lda SYSCALL_TABLE+1,y
  sta __rc5
  lda CPU_STACK+2,x
  sta __rc2
  lda CPU_STACK+1,x
  sta __rc3
#else
  tay
  ldx CPU_STACK+1,y
  lda SYSCALL_TABLE,x
  sta __rc4
  lda SYSCALL_TABLE+1,x
  sta __rc5
  lda CPU_STACK+3,y
  sta __rc2
  lda CPU_STACK+2,y
  sta __rc3
#endif
  jsr .Lbrk_dispatch

;
; Restore the user stack pointer from the "current_proc" variable.
; Also arrange to pass A:X back to the caller.
;
#if defined(CPU_65C02)
  tay
  stx __rc3
  lda (current_proc)
  tax
  txs
  pla ; Discard the incoming X value from the stack.
  pla ; Discard the incoming A value from the stack.
  tya
  ldx __rc3
#else
  sta __rc2
  stx __rc3
  ldy #0
  lda (current_proc),y
  tax
  txs
  pla ; Discard the incoming Y value from the stack.
  pla ; Discard the incoming X value from the stack.
  pla ; Discard the incoming A value from the stack.
  lda __rc2
  ldx __rc3
#endif

;
; Now leaving kernel space.  Re-enable preemption and return.
;
  dec in_kernel
  rti
.Lbrk_dispatch:
  jmp (__rc4)

;
; Swap out the current process.
;
; Everything from S up to PROC_STACK_SIZE is copied out of the
; 6502 return stack at $0100 to the process block.
;
.global swap_out
.section .text.swap_out,"ax",@progbits
swap_out:
#if defined(CPU_65C02)
  lda (current_proc)
#else
  ldy #0
  lda (current_proc),y
#endif
  clc
  adc #4
  tay
.Lswap_out_loop:
  cpy #(PROC_STACK_SIZE+3)
  bcs .Lswap_out_done
  lda CPU_STACK-3,y
  sta (current_proc),y
  iny
  bne .Lswap_out_loop
.Lswap_out_done:
  ; Set current_proc to NULL before returning.
#if defined(CPU_65C02)
  stz current_proc
  stz current_proc+1
#else
  lda #0
  sta current_proc
  sta current_proc+1
#endif
  rts

;
; Swap in a process and start running it.
;
; Everything from S up to PROC_STACK_SIZE is copied out of the
; process block to the 6502 return stack at $0100.
;
.global swap_in
.section .text.swap_in,"ax",@progbits
swap_in:
  lda __rc2
  ldx __rc3
  cmp current_proc      ; Is the new "proc" the same as "current_proc"?
  bne .Lswap_in_new     ; If so, the user stack is already swapped in.
  cpx current_proc+1
  beq .Lswap_in_ready
;
.Lswap_in_new:
  sta current_proc      ; We are swapping in a new process.
  stx current_proc+1
  ldy #0
  lda (current_proc),y
  clc
  adc #4
  tay
;
.Lswap_in_loop:
  cpy #(PROC_STACK_SIZE+3)
  bcs .Lswap_in_ready
  lda (current_proc),y
  sta CPU_STACK-3,y
  iny
  bne .Lswap_in_loop
;
.Lswap_in_ready:
;
; Switch to the target process's stack.
;
#if defined(CPU_65C02)
  lda (current_proc)
#else
  ldy #0
  lda (current_proc),y
#endif
  tax
  txs

;
; Pop the stack frame, restoring the A:X value we want to return
; to the interrupted process.
;
#if defined(CPU_65C02)
  plx
  pla
#else
  pla
  tay
  pla
  tax
  pla
#endif

;
; Now leaving kernel space.  Re-enable preemption and return.
;
  dec in_kernel
  rti

.section .bss,"aw",@nobits
__kernel_stack:
  .fill 3
