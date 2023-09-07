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

    /* Put the kernel and its runtime data in the top part of memory.
     * User space programs will be loaded to the bottom half of memory. */
    ram (rw) : ORIGIN = 0xc000, LENGTH = 0x3ff0
}

REGION_ALIAS("c_readonly", ram)
REGION_ALIAS("c_writeable", ram)

SECTIONS { INCLUDE c.ld }

/* Set initial soft stack address to just above last memory address. (It grows down.) */
__stack = 0xfff0;

/* Tell the kernel where its usable user space RAM lies */
user_space_ram_start = 0x0200;
user_space_ram_end = 0x7000;

/* Tell the kernel where the buffer cache is located in RAM */
buffer_cache_ram_start = 0x7000;
buffer_cache_ram_end = 0x8000;

/* Tell the kernel where the shell code lies */
shell_start = 0x8000;

OUTPUT_FORMAT {
    SHORT(0xc000)
    SHORT(__data_end - 0xc000)
    TRIM(ram)

    SHORT(0xfffa)
    SHORT(6)
    SHORT(0)
    SHORT(_start)
    SHORT(_irqbrk)
}
