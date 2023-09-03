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
#include <mosnix/config.h>

#define CPU_STACK 0x0100

.global sched_start
.section .text.sched_start,"ax",@progbits
sched_start:
;
; Adjust the top of stack so that the main kernel thread is using the
; right stack range to be converted into the shell user process.
;
  ldx #CONFIG_RETURN_STACK_SIZE
  txs
;
; We are currently in the kernel context.
;
  inc mos8(in_kernel)
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
  inc mos8(in_kernel)

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
  dec mos8(in_kernel)
  rti
.Lbrk_dispatch:
  jmp (__rc4)

;
; Switch to another process and continue running it.
;
.global proc_switch_to
.section .text.proc_switch_to,"ax",@progbits
proc_switch_to:
;
; Is this the same process as last time?  If so, then we are already in
; the right process and there is nothing to do.
;
  lda __rc2
  cmp current_proc
  bne .Lswap_out_old
  lda __rc3
  cmp current_proc+1
  bne .Lswap_out_old
  rts

;
; If "current_proc" is NULL, then there is nothing to "swap out".
;
.Lswap_out_old:
  lda current_proc
  ora current_proc+1
  beq .Lswap_in_new

;
; Set up the return conditions so that "rti" from another process
; will return control to this process.
;
  lda #mos16hi(.Lswap_in_return)
  pha
  lda #mos16lo(.Lswap_in_return)
  pha
  php

;
; Copy the contents of the return address stack to the process block.
;
  tsx
  txa
#if defined(CPU_65C02)
  sta (current_proc)
#else
  ldy #0
  sta (current_proc),y
#endif
  clc
  adc #4
  tay
.Lswap_out_ustack_loop:
  cpy #(CONFIG_RETURN_STACK_SIZE+3)
  bcs .Lswap_out_ustack_done
  lda CPU_STACK-3,y
  sta (current_proc),y
  iny
  bne .Lswap_out_ustack_loop
.Lswap_out_ustack_done:

;
; Save RC0:RC1 as the "kstack" pointer in the process block.
;
  lda __rc0
  sta (current_proc),y
  iny
  lda __rc1
  sta (current_proc),y

;
; Copy RC20..RC31 from the actual registers to the process block.
;
  ldx #0
.Lswap_out_imag_regs:
  iny
  lda (current_proc),y
  sta __rc12,x
  inx
  cpx #12
  bne .Lswap_out_imag_regs

;
; Set the process that we are switching to.
;
.Lswap_in_new:
  lda __rc2
  sta current_proc
  lda __rc3
  sta current_proc+1

;
; Copy the saved stack contents from the process block to the actual stack.
;
#if defined(CPU_65C02)
  lda (current_proc)
#else
  ldy #0
  lda (current_proc),y
#endif
  clc
  adc #4
  tay
;
.Lswap_in_ustack_loop:
  cpy #(CONFIG_RETURN_STACK_SIZE+3)
  bcs .Lswap_in_kstack
  lda (current_proc),y
  sta CPU_STACK-3,y
  iny
  bne .Lswap_in_ustack_loop

;
; Modify the kernel's RC0:RC1 to point at the kernel data stack
; for the new process.  This way, the kernel's data stack is
; ready to use when the user process executes a system call.
;
.Lswap_in_kstack:
  iny
  lda (current_proc),y
  dey
  ora (current_proc),y
  beq .Lswap_in_set_kstack

;
; Use the "kstack" value from the process block as the new RC0:RC1.
;
  lda (current_proc),y
  sta __rc0
  iny
  lda (current_proc),y
  sta __rc1

;
; Copy RC20..RC31 from the process block to the actual registers so
; that they are ready to use when the next system call happens.
;
  ldx #0
.Lswap_in_imag_regs:
  iny
  lda (current_proc),y
  sta __rc12,x
  inx
  cpx #12
  bne .Lswap_in_imag_regs
  beq .Lswap_in_switch_stack

;
; The "kstack" value is NULL so this is the first process we have launched.
; Copy the current top-of stack in RC0:RC1 to "kstack" in the process block.
; The original kernel data stack will now become this process's stack.
;
.Lswap_in_set_kstack:
  lda __rc0
  sta (current_proc),y
  iny
  lda __rc1
  sta (current_proc),y

;
; Switch to the target process's stack.
;
.Lswap_in_switch_stack:
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
  dec mos8(in_kernel)
  rti

;
; Control comes back to here if another process has switched back to us
; with the "rti" instruction above.  Disable preemption and return back to
; the code that called proc_switch_to().
;
.Lswap_in_return:
  inc mos8(in_kernel)
  rts
