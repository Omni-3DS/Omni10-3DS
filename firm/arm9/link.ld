OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

MEMORY
{
    ram (rwx) : ORIGIN = 0x08000040, LENGTH = 96K - 0x40
}

SECTIONS
{
    .text :
    {
        KEEP(*(.text.startup))
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
    } > ram

    .data :
    {
        *(.data*)
        . = ALIGN(4);
    } > ram

    .bss :
    {
        *(.bss*)
        *(COMMON)
        . = ALIGN(8);
    } > ram

    /DISCARD/ :
    {
        *(.comment)
        *(.ARM.attributes)
    }
}
