; Copyright (c) 2023 Rhys Weatherley
;
; Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
; See https://github.com/llvm-mos/llvm-mos-sdk/blob/main/LICENSE for license
; information.

.include "imag.inc"

; Save argc and argv early in the startup sequence.
.global __save_args
.section .init.50,"axR",@progbits
__save_args:
  sta __argc
  stx __argc+1
  lda __rc2
  ldx __rc3
  sta __argv
  stx __argv+1

; Restore argc and argv just before calling main().
.global __restore_args
.section .init.900,"axR",@progbits
__restore_args:
  lda __argv
  ldx __argv+1
  sta __rc2
  stx __rc3
  lda __argc
  ldx __argc+1

; Storage for argc and argv in global memory.
.global __argc
.global __argv
.section .noinit,"aw",@nobits
__argc:
  .fill 2
__argv:
  .fill 2
