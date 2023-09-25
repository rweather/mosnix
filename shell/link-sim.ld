/* 6502 simulator linker script for the shell.
 *
 * Based on the linker script from LLVM-MOS-SDK.
 */

/* Provide imaginary (zero page) registers. */
__rc0 = 0x40;
INCLUDE imag-regs.ld
ASSERT(__rc31 == 0x005f, "Inconsistent zero page map.")

MEMORY {
    /* Shell can use 0x40 to 0x5f in the zero page, but no more */
    zp : ORIGIN = __rc31 + 1, LENGTH = 0

    /* Arrange to put shell's data in RAM starting at 0x7800 */
    ram : ORIGIN = 0x7800, LENGTH = 0x800

    /* Put the shell's code into the top half of memory */
    rom : ORIGIN = 0x8000, LENGTH = 0x3000
}

REGION_ALIAS("c_readonly", rom)
REGION_ALIAS("c_writeable", ram)

SECTIONS { INCLUDE c.ld }

__stack = ORIGIN(ram) + LENGTH(ram);

OUTPUT_FORMAT {
    SHORT(0x8000)
    SHORT(0x3000)
    FULL(rom)
}
