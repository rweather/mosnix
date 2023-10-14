; Copyright (c) 2023 Rhys Weatherley
;
; Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
; See https://github.com/llvm-mos/llvm-mos-sdk/blob/main/LICENSE for license
; information.

.include "imag.inc"

#define VIA_T2CL    0x6008
#define VIA_T2CH    0x6009
#define VIA_ACR     0x600b
#define VIA_IFR     0x600d
#define VIA_IER     0x600e

; Roughly 1/256'th of a second at 1MHz.
;
; Will gain about 5.5 seconds per day or 64ppm.  However, the 1MHz can
; oscillator used in the Ben Eater breadboard design is 200ppm so the
; error in the counter is still less than the error in the oscillator!
#define T2COUNT     3906

; Initialize the system tick timer.
.global __do_init_systick
.section .init.250,"axR",@progbits
__do_init_systick:
  lda #0                        ; Zero the system tick counter at startup.
  sta __systick_value
  sta __systick_value+1
  sta __systick_value+2
  sta __systick_value+3
  sta __systick_value+4
  sta __systick_value+5
  lda #$a0                      ; Enable T2 interrupts.
  sta VIA_IER
  lda #$00                      ; T2 in one-shot mode.
  sta VIA_ACR
  lda #mos16lo(T2COUNT)         ; T2 interrupt every T2COUNT clock ticks.
  sta VIA_T2CL
  lda #mos16hi(T2COUNT)
  sta VIA_T2CH

; Handle interrupts for the system tick timer.
.text
.global __systick_isr
.section .text.__systick_isr,"axR",@progbits
__systick_isr:
  lda VIA_IFR                   ; Has the T2 timer expired?
  and #$20
  beq .L__systick_isr_end
  inc __systick_value           ; Increment the system tick counter.
  bne .L__systick_isr_restart
  inc __systick_value+1
  bne .L__systick_isr_restart
  inc __systick_value+2
  bne .L__systick_isr_restart
  inc __systick_value+3
  bne .L__systick_isr_restart
  inc __systick_value+4
.L__systick_isr_restart:
  ldx VIA_T2CH                  ; Restart the one-shot T2 timer.
  lda VIA_T2CL
  cpx VIA_T2CH                  ; Has the high byte changed?
  bne .L__systick_isr_restart   ; If yes, we need to read T2CL/T2CH again.
  clc
  adc #mos16lo(T2COUNT - 24)    ; Adjust the T2 deadline for the elapsed ticks.
  sta VIA_T2CL
  txa
  adc #mos16hi(T2COUNT - 24)
  sta VIA_T2CH
.L__systick_isr_end:
  rts

; Get the value of the system tick timer, which is a 40-bit value.
.global sys_monoclock
.section .text.sys_monoclock,"ax",@progbits
sys_monoclock:
  ldy #0
  sei
  lda __systick_value
  sta (__rc2),y
  iny
  lda __systick_value+1
  sta (__rc2),y
  iny
  lda __systick_value+2
  sta (__rc2),y
  iny
  lda __systick_value+3
  sta (__rc2),y
  iny
  lda __systick_value+4
  cli
  sta (__rc2),y
  iny
  lda #0
  sta (__rc2),y
  iny
  sta (__rc2),y
  iny
  sta (__rc2),y
  rts

; Get the low 16 bits of the system tick timer.
.global sys_clock
.section .text.sys_clock,"ax",@progbits
sys_clock:
  sei
  lda __systick_value
  ldx __systick_value+1
  cli
  rts

.section .noinit,"aw",@nobits
__systick_value:
  .fill 5
