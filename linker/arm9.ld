/*
 * Omni10-3DS ARM9 linker script
 * See docs/MEMORY_MAP.md
 * ARM9 internal RAM: 0x08000000, 1MB
 */

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

MEMORY
{
    ARM9RAM (rwx) : ORIGIN = 0x08000000, LENGTH = 1024K
}

SECTIONS
{
    .text : {
        KEEP(*(.text.start))
        *(.text*)
        *(.rodata*)
    } > ARM9RAM

    .data : {
        *(.data*)
    } > ARM9RAM

    .bss : {
        *(.bss*)
        *(COMMON)
    } > ARM9RAM

    __stack_top = ORIGIN(ARM9RAM) + LENGTH(ARM9RAM);
}
