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
  rom : ORIGIN = 0x8000, LENGTH = 0x3000
}

REGION_ALIAS("c_readonly", rom)
REGION_ALIAS("c_writeable", ram)

SECTIONS { INCLUDE c.ld }

/* Set initial soft stack address to just above last ram address. (It grows down.) */
__stack = ORIGIN(ram) + LENGTH(ram);

OUTPUT_FORMAT {
  FULL(rom)
}
