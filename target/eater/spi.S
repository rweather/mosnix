; Copyright (c) 2023 Rhys Weatherley
;
; Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
; See https://github.com/llvm-mos/llvm-mos-sdk/blob/main/LICENSE for license
; information.

; These routines implement the CPOL=0, CPHA=0 SPI configuration only.
;
; Thanks to Ben Eater for showing how to use SCLK=0, MOSI=1, MISO=6 to
; make SPI more efficient: https://www.youtube.com/watch?v=MCi7dCBhVpQ

.include "imag.inc"

; Use PORTA on the VIA for the SPI interface.
#define SPI_PORT 0x6001
#define SPI_DDR  0x6003

; The SPI pins are assigned as follows:
;
; PA0 - SCLK
; PA1 - MOSI
; PA5 - CS for the SD card
; PA6 - MISO

.global spi_init
.section .text.spi_init,"ax",@progbits
spi_init:
  lda SPI_PORT
  and #$FC                  ; Set MOSI and SCLK low.
  ora #$20                  ; Set CS high.
  sta SPI_PORT
  lda SPI_DDR
  ora #$23                  ; Set MOSI, SCLK, and CS to be outputs.
  and #$BF                  ; Set MISO to be an input.
  sta SPI_DDR
  rts

.global spi_send_byte
.section .text.spi_send_byte,"ax",@progbits
spi_send_byte:
  phx                       ; Preserve X as the caller may be using it.
  sta __rc5                 ; Save the value to be sent.
  lda #2
  ldx #8
.Lspi_send_bit:
  asl __rc5                 ; Is the next bit a 0 or a 1?
  bcc .Lspi_send_bit_0
  tsb SPI_PORT              ; Turn on MOSI.
  inc SPI_PORT              ; Raise and lower SCLK.
  dec SPI_PORT
  dex
  bne .Lspi_send_bit
  plx
  rts
.Lspi_send_bit_0:
  trb SPI_PORT              ; Turn off MOSI.
  inc SPI_PORT              ; Raise and lower SCLK.
  dec SPI_PORT
  dex
  bne .Lspi_send_bit
  plx
  rts

.global spi_receive_byte
.section .text.spi_receive_byte,"ax",@progbits
spi_receive_byte:
  phx                       ; Preserve X as the caller may be using it.
  stz __rc5
  lda #2
  tsb SPI_PORT              ; Turn on MOSI so that we send a $FF byte.
  ldx #8
  sei                       ; Disable interrupts for precise timing.
.Lspi_receive_bit:
  asl __rc5
  inc SPI_PORT              ; Raise SCLK.
  bit SPI_PORT              ; Read the state of MISO.
  dec SPI_PORT              ; Lower SCLK.
  bvc .Lspi_receive_bit_0
  inc __rc5                 ; MISO is 1.
.Lspi_receive_bit_0:
  dex
  bne .Lspi_receive_bit
  cli                       ; Re-enable interrupts.
  plx
  lda __rc5
  rts

.global spi_send
.section .text.spi_send,"ax",@progbits
spi_send:
  sta __rc4
  ldy #0
  cpx #0
  beq .Lspi_send_2
.Lspi_send_1:
  lda (__rc2),y
  jsr spi_send_byte
  iny
  bne .Lspi_send_1
  inc __rc3
  dex
  bne .Lspi_send_1
.Lspi_send_2:
  ldx __rc4
  beq .Lspi_send_done
.Lspi_send_3:
  lda (__rc2),y
  jsr spi_send_byte
  iny
  dex
  bne .Lspi_send_3
.Lspi_send_done:
  rts

.global spi_receive
.section .text.spi_receive,"ax",@progbits
spi_receive:
  sta __rc4
  ldy #0
  cpx #0
  beq .Lspi_receive_2
.Lspi_receive_1:
  jsr spi_receive_byte
  sta (__rc2),y
  iny
  bne .Lspi_receive_1
  inc __rc3
  dex
  bne .Lspi_receive_1
.Lspi_receive_2:
  ldx __rc4
  beq .Lspi_receive_done
.Lspi_receive_3:
  jsr spi_receive_byte
  sta (__rc2),y
  iny
  dex
  bne .Lspi_receive_3
.Lspi_receive_done:
  rts

.global spi_blank
.section .text.spi_blank,"ax",@progbits
spi_blank:
  sta __rc4
  cpx #0
  beq .Lspi_blank_2
  ldy #0
.Lspi_blank_1:
  jsr spi_receive_byte
  iny
  bne .Lspi_blank_1
  dex
  bne .Lspi_blank_1
.Lspi_blank_2:
  ldx __rc4
  beq .Lspi_blank_done
.Lspi_blank_3:
  lda #$FF
  jsr spi_receive_byte
  dex
  bne .Lspi_blank_3
.Lspi_blank_done:
  rts

.global spi_sdcard_lower_cs
.section .text.spi_sdcard_lower_cs,"ax",@progbits
spi_sdcard_lower_cs:
  lda #$20
  trb SPI_PORT
  rts

.global spi_sdcard_raise_cs
.section .text.spi_sdcard_raise_cs,"ax",@progbits
spi_sdcard_raise_cs:
  lda #$20
  tsb SPI_PORT
  rts
