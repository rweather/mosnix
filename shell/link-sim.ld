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

    /* Put the shell and its runtime data in the top part of memory */
    ram (rw) : ORIGIN = 0x8000, LENGTH = 0x2000
}

SECTIONS { INCLUDE c.ld }

__stack = ORIGIN(ram) + LENGTH(ram);

OUTPUT_FORMAT {
    SHORT(0x8000)
    SHORT(0x2000)
    FULL(ram)
}
