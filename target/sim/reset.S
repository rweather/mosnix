; Copyright (c) 2023 Rhys Weatherley
;
; Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
; See https://github.com/llvm-mos/llvm-mos-sdk/blob/main/LICENSE for license
; information.

.include "imag.inc"

; Initialize the system from the reset vector.
.global __do_reset
.section .init.50,"axR",@progbits
__do_reset:
; Fix the D and I flags.
  cld
  cli
; Set up the initial 6502 stack pointer.
  ldx #$ff
  txs

.text
; IRQBRK handler.
.global _irqbrk
.section .text._irqbrk,"axR",@progbits
_irqbrk:
  pha                   ; Save A and X on the stack.
  txa
  pha
  cld                   ; Just in case.
  tsx
  lda $0102,x           ; Was this due to a BRK instruction?
  and #$10
  beq .Lno_break
  tya                   ; Also save Y on the stack.
  pha
  cli                   ; Re-enable interrupts.
  jmp brk_syscall       ; Handle the system call.
.Lno_break:
  pla
  tax
  pla
  rti
