; Copyright (c) 2023 Rhys Weatherley
;
; Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
; See https://github.com/llvm-mos/llvm-mos-sdk/blob/main/LICENSE for license
; information.

.include "imag.inc"

;
; LLVM-MOS can struggle with 32-bit shift operations, which can make the
; SD card code very large.  Provide some helpers in assembly code to help
; reduce the size to something more reasonable.
;

; Offsets into the sd_info structure.
#define ofs_detect          0
#define ofs_sdhc            1
#define ofs_cluster_size    2
#define ofs_cluster_shift   3
#define ofs_part_offset     4
#define ofs_part_size       8
#define ofs_fat1            12
#define ofs_fat2            16
#define ofs_fat_size        20
#define ofs_root            24
#define ofs_cluster_offset  28
#define ofs_cluster_count   32
#define ofs_cache_block     36
#define ofs_cache_block2    40
#define ofs_cache_mode      44
#define ofs_block           45

;
; uint32_t sd_cluster_to_block(uint32_t cluster)
;
; Input and output values are in A:X:rc2:rc3.
;
.global sd_cluster_to_block
.section .text.sd_cluster_to_block,"ax",@progbits
sd_cluster_to_block:
  ;
  ; if (cluster < 2)
  ;     return 0;
  ; cluster -= 2;
  ;
  sec
  sbc #2
  sta __rc4
  txa
  sbc #0
  sta __rc5
  lda __rc2
  sbc #0
  sta __rc2
  lda __rc3
  sbc #0
  sta __rc3
  bcc .Lsd_cluster_to_block_error
  ;
  ; if (cluster >= sd_info.cluster_count)
  ;     return 0;
  ;
  cmp sd_info+ofs_cluster_count+3
  bcc .Lsd_cluster_to_block_ok
  bne .Lsd_cluster_to_block_error
  lda __rc2
  cmp sd_info+ofs_cluster_count+2
  bcc .Lsd_cluster_to_block_ok
  bne .Lsd_cluster_to_block_error
  lda __rc5
  cmp sd_info+ofs_cluster_count+1
  bcc .Lsd_cluster_to_block_ok
  bne .Lsd_cluster_to_block_error
  lda __rc4
  cmp sd_info+ofs_cluster_count
  bcs .Lsd_cluster_to_block_error
.Lsd_cluster_to_block_ok:
  ;
  ; cluster <<= sd_info.cluster_shift;
  ;
  ldx sd_info+ofs_cluster_shift
  beq .Lsd_cluster_to_block_shift_done
.Lsd_cluster_to_block_shift:
  asl __rc4
  rol __rc5
  rol __rc2
  rol __rc3
  dex
  bne .Lsd_cluster_to_block_shift
  ;
  ; cluster += sd_info.cluster_offset
  ;
.Lsd_cluster_to_block_shift_done:
  lda __rc4
  clc
  adc sd_info+ofs_cluster_offset
  sta __rc4
  lda __rc5
  adc sd_info+ofs_cluster_offset+1
  tax
  lda __rc2
  adc sd_info+ofs_cluster_offset+2
  sta __rc2
  lda __rc3
  adc sd_info+ofs_cluster_offset+3
  sta __rc3
  lda __rc4
  rts
.Lsd_cluster_to_block_error:
  lda #0
  tax
  sta __rc2
  sta __rc3
  rts

;
; uint32_t sd_get_size(void)
;
; Get the size of the SD card in blocks.
;
.global sd_get_size
.section .text.sd_get_size,"ax",@progbits
sd_get_size:
  lda sd_info+ofs_block         ; Check the CSD version.
  and #$C0
  beq .Lsd_get_size_v1
  cmp #$40
  beq .Lsd_get_size_v2
  lda #0                        ; Unknown version, return zero.
  tax
  sta __rc2
  sta __rc3
  rts
;
.Lsd_get_size_v2:
;
; Version 2: (C_SIZE + 1) << 10
;
  lda sd_info+ofs_block+9       ; Calculate (C_SIZE + 1) << 8.
  clc
  adc #1
  sta __rc4
  lda sd_info+ofs_block+8
  adc #0
  sta __rc2
  lda sd_info+ofs_block+7
  adc #0
  sta __rc3
;
  asl __rc4                     ; Shift up by another 2 for "<< 10".
  rol __rc2
  rol __rc3
  asl __rc4
  rol __rc2
  rol __rc3
  lda #0
  ldx __rc4
  rts
;
.Lsd_get_size_v1:
;
; Version 1: (C_SIZE + 1) << (C_SIZE_MULT + READ_BL_LEN - 7)
;
  sta __rc2                     ; Set rc4:rc5:rc2:rc3 to (CSIZE + 1).
  sta __rc3
  asl sd_info+ofs_block+8       ; CSD is destroyed, but we don't need it
  rol sd_info+ofs_block+7       ; after this function finishes.
  rol sd_info+ofs_block+6
  asl sd_info+ofs_block+8
  lda sd_info+ofs_block+7
  rol
  sta __rc4
  lda sd_info+ofs_block+6
  rol
  and #$0F
  sta __rc5
  lda __rc4
  clc
  adc #1
  sta __rc4
  lda __rc5
  adc #0
  sta __rc5
;
  lda sd_info+ofs_block+9       ; rc6 = C_SIZE_MULT
  asl sd_info+ofs_block+10
  rol
  and #$07
  sta __rc6
  lda sd_info+ofs_block+5       ; A = READ_BL_LEN
  and #$0F
  clc
  adc __rc6                     ; A = C_SIZE_MULT + READ_BL_LEN
  sec
  sbc #7                        ; A = C_SIZE_MULT + READ_BL_LEN - 7
  beq .Lsd_get_size_shift_done
;
  tax
.Lsd_get_size_shift:
  asl __rc4
  rol __rc5
  rol __rc2
  rol __rc3
  dex
  bne .Lsd_get_size_shift
;
.Lsd_get_size_shift_done:
  lda __rc4
  ldx __rc5
  rts
