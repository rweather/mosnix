;
; Copyright (c) 2023 Rhys Weatherley
;
; Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
; See https://github.com/rweater/mosnix/blob/main/LICENSE for license
; information.
;

#include "imag.inc"

;
; See "doc/syscalls.dox" for a description of what this is doing.
;
.global syscall
.section .text.syscall,"ax",@progbits
syscall:
  asl                   ; Get the system call address at SYSCALL + A * 2.
  tax
  lda SYSCALL,x
  sta __rc2
  lda SYSCALL+1,x
  sta __rc3
  lda __rc0
  ldx __rc1
  jsr .Lsyscall_invoke  ; Invoke the system call.
  cpx #$80              ; Is the result negative?
  bcs .Lsyscall_error
  rts                   ; Return the system call result in A:X.
.Lsyscall_error:
  eor #$ff              ; Store -A:X into errno.
  clc
  adc #1
  sta errno
  txa
  eor #$ff
  adc #0
  sta errno+1
  lda #$ff              ; Return -1 in A:X.
  tax
  rts
.Lsyscall_invoke:
  jmp (__rc2)

; Declare the "errno" variable for the user space application.
.global errno
.section .bss,"aw",@nobits
errno:
  .fill 2
