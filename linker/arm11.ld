/*
 * Omni10-3DS ARM11 linker script
 * See docs/MEMORY_MAP.md
 * AXI WRAM: 0x1FF80000, 512KB
 */

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start_arm11)

MEMORY
{
    AXIWRAM (rwx) : ORIGIN = 0x1FF80000, LENGTH = 512K
}

SECTIONS
{
    .text : {
        KEEP(*(.text.start))
        *(.text*)
        *(.rodata*)
    } > AXIWRAM

    .data : {
        *(.data*)
    } > AXIWRAM

    .bss : {
        *(.bss*)
        *(COMMON)
    } > AXIWRAM

    __stack_top = ORIGIN(AXIWRAM) + LENGTH(AXIWRAM);
}
