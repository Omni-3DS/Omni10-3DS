/* Omni10-3DS ARM9 linker script (skeleton) */

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

MEMORY
{
    /* Placeholder regions – real 3DS ARM9 map later */
    RAM (rwx) : ORIGIN = 0x08000000, LENGTH = 1M
}

SECTIONS
{
    .text : {
        *(.text.start)
        *(.text*)
    } > RAM

    .rodata : { *(.rodata*) } > RAM
    .data : { *(.data*) } > RAM
    .bss : { *(.bss*) *(COMMON) } > RAM
}
