;
; Copyright (C) 2023 Southern Storm Software, Pty Ltd.
;
; Permission is hereby granted, free of charge, to any person obtaining a
; copy of this software and associated documentation files (the "Software"),
; to deal in the Software without restriction, including without limitation
; the rights to use, copy, modify, merge, publish, distribute, sublicense,
; and/or sell copies of the Software, and to permit persons to whom the
; Software is furnished to do so, subject to the following conditions:
;
; The above copyright notice and this permission notice shall be included
; in all copies or substantial portions of the Software.
;
; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
; OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
; FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
; AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
; LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
; FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
; DEALINGS IN THE SOFTWARE.
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
