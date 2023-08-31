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
  ram : ORIGIN = 0x200, LENGTH = 0x1000 - 0x200

  /* Put the kernel's code into ROM */
  rom : ORIGIN = 0x8000, LENGTH = 0x7ffa
}

/* Provide default IRQ and NMI handlers if the program doesn't have them. */
PROVIDE(irq = _irq_default);
PROVIDE(nmi = _irq_default);

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
  SHORT(nmi)
  SHORT(_start)
  SHORT(_irqbrk)
}
