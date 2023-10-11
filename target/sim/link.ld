/* 6502 simulator linker script.
 *
 * Produces a 64KiB memory image file usable with the provided simulator.
 *
 * Based on the linker script from LLVM-MOS-SDK.
 */

/* Provide imaginary (zero page) registers. */
__rc0 = 0x00;
INCLUDE imag-regs.ld
ASSERT(__rc31 == 0x001f, "Inconsistent zero page map.")

MEMORY {
    /* Kernel can use 0x00 to 0x3f in the zero page, but no more.
     * The rest of the zero page is reserved for user space programs. */
    zp : ORIGIN = __rc31 + 1, LENGTH = 0x20

    /* First 4K of memory is reserved for the kernel.  The rest is
     * used by user space applications. */
    ram : ORIGIN = 0x200, LENGTH = 0x1000 - 0x200

    /* Put the kernel code in the top part of memory. */
    rom : ORIGIN = 0xb000, LENGTH = 0x4ff0
}

REGION_ALIAS("c_readonly", rom)
REGION_ALIAS("c_writeable", ram)

SECTIONS { INCLUDE c.ld }

/* Set initial soft stack address to just above last memory address. (It grows down.) */
__stack = ORIGIN(ram) + LENGTH(ram);

/* Tell the kernel where its usable user space RAM lies */
user_space_ram_start = 0x1000;
user_space_ram_end = 0x7800;

/* Tell the kernel where the shell code lies */
shell_start = 0x8000;

OUTPUT_FORMAT {
    SHORT(0xb000)
    SHORT(0x4ff0)
    FULL(rom)

    SHORT(0xfffa)
    SHORT(6)
    SHORT(0)
    SHORT(_start)
    SHORT(_irqbrk)
}
