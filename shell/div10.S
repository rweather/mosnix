;
; Copyright (c) 2023 Rhys Weatherley
;
; Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
; See https://github.com/rweater/mosnix/blob/main/LICENSE for license
; information.
;

#include "imag.inc"

;
; Divides a value by 10, returns the remainder, and replaces the
; original value with the quotient.
;
; unsigned char div10(unsigned long *value);
;
.global div10
.section .text.div10,"ax",@progbits
div10:
  ;
  ; Copy the incoming value to the registers rc4:rc5:rc6:rc7.
  ; A is set to zero as the top-most byte of a 40-bit working register.
  ;
  ldy #0
  tya
  lda (__rc2),y
  sta __rc4
  iny
  lda (__rc2),y
  sta __rc5
  iny
  lda (__rc2),y
  sta __rc6
  iny
  lda (__rc2),y
  sta __rc7
;
  ldy #32
.Ldiv10_loop:
  ;
  ; Rotate the 40-bit working register left.
  ;
  asl __rc4
  rol __rc5
  rol __rc6
  rol __rc7
  rol a
  ;
  ; Compare the top-most byte with 10.  If it is greater than or
  ; equal to 10, then this bit of the result is 1.
  ;
  cmp #10
  bcc .Ldiv10_zero_bit
  ;
  ; Subtract 10 from the top-most byte.
  ;
  sbc #10
  ;
  ; Set the bottom-most bit of the result to 1.
  ;
  inc __rc4
.Ldiv10_zero_bit:
  dey
  bne .Ldiv10_loop
  ;
  ; The registers rc4:rc5:rc6:rc7 contain the quotient.  A is the remainder.
  ;
  pha
  lda __rc4
  sta (__rc2),y
  iny
  lda __rc5
  sta (__rc2),y
  iny
  lda __rc6
  sta (__rc2),y
  iny
  lda __rc7
  sta (__rc2),y
  pla
  rts
