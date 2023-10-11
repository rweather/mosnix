/* Linker script for Ben Eater's 6502 breadboard computer. */

/* Provide imaginary (zero page) registers. */
__rc0 = 0x00;
INCLUDE imag-regs.ld
ASSERT(__rc31 == 0x1f, "Inconsistent zero page map.")

MEMORY {
  /* Kernel can use 0x00 to 0x3f in the zero page, but no more.
   * The rest of the zero page is reserved for user space programs. */
  zp : ORIGIN = __rc31 + 1, LENGTH = 0x20

  /* First 4K of memory is reserved for the kernel.  The rest is
   * used by user space applications. */
  ram : ORIGIN = 0x200, LENGTH = 0x1800 - 0x200

  /* Put the kernel's code into ROM */
  rom : ORIGIN = 0xa800, LENGTH = 0x57fa
}

/* Provide default IRQ and NMI handlers if the program doesn't have them. */
PROVIDE(irq = _irq_default);
PROVIDE(nmi = _irq_default);

/* Tell the kernel where its usable user space RAM lies */
user_space_ram_start = 0x1800;
user_space_ram_end = 0x3800;

/* Tell the kernel where the in-ROM shell code lies */
shell_start = 0x8000;

REGION_ALIAS("c_readonly", rom)
REGION_ALIAS("c_writeable", ram)

SECTIONS { INCLUDE c.ld }

/* Set initial soft stack address to just above last ram address. (It grows down.) */
__stack = ORIGIN(ram) + LENGTH(ram);

OUTPUT_FORMAT {
  FULL(rom)
  SHORT(nmi)
  SHORT(_start)
  SHORT(_irqbrk)
}
