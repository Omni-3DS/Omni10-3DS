/* Omni10-3DS ARM11 linker script (skeleton) */

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start_arm11)

MEMORY
{
    /* Placeholder – real ARM11 memory map later */
    RAM (rwx) : ORIGIN = 0x1FF80000, LENGTH = 512K
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
