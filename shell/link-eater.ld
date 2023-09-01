/* Shell linker script for Ben Eater's 6502 breadboard computer. */

/* Provide imaginary (zero page) registers. */
__rc0 = 0x40;
INCLUDE imag-regs.ld
ASSERT(__rc31 == 0x5f, "Inconsistent zero page map.")

MEMORY {
  /* Shell can use 0x40 to 0x5f in the zero page, but no more */
  zp : ORIGIN = __rc31 + 1, LENGTH = 0

  /* Arrange to put shell's data in RAM starting at 0x3800 */
  ram : ORIGIN = 0x3800, LENGTH = 0x800

  /* Put the shell's code into ROM */
  rom : ORIGIN = 0x8000, LENGTH = 0x2000
}

SECTIONS {
  .text : {
       INCLUDE text-sections.ld
  } >rom AT>rom
  .rodata : { INCLUDE rodata-sections.ld } >rom AT>rom
  .data : { INCLUDE data-sections.ld } >ram AT>rom
  __data_load_start = LOADADDR(.data) - LOADADDR(.rodata) + ADDR(.rodata);
  __data_size = SIZEOF(.data);
  .zp.data : { INCLUDE zp-data-sections.ld } >zp AT>rom
  __zp_data_load_start = LOADADDR(.zp.data) - LOADADDR(.rodata) + ADDR(.rodata);
  __zp_data_size = SIZEOF(.zp.data);
  .zp.bss (NOLOAD) : {
    INCLUDE zp-bss-sections.ld
  } >zp
  INCLUDE zp-bss-symbols.ld
  INCLUDE zp-noinit.ld
  .bss (NOLOAD) : { INCLUDE bss-sections.ld } >ram AT>ram
  INCLUDE bss-symbols.ld
  .noinit (NOLOAD) : { INCLUDE noinit-sections.ld } >ram AT>ram
}

/* Set initial soft stack address to just above last ram address. (It grows down.) */
__stack = ORIGIN(ram) + LENGTH(ram);

OUTPUT_FORMAT {
  FULL(rom)
}
