OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(main)

MEMORY
{
    ram (rwx) : ORIGIN = 0x08000000, LENGTH = 96K
}

SECTIONS
{
    .text :
    {
        *(.text.startup)
        *(.text)
        *(.text.*)
        *(.rodata)
        *(.rodata.*)
        . = ALIGN(4);
    } > ram

    .data :
    {
        *(.data)
        *(.data.*)
        . = ALIGN(4);
    } > ram

    .bss :
    {
        *(.bss)
        *(.bss.*)
        *(COMMON)
        . = ALIGN(4);
    } > ram

    /DISCARD/ :
    {
        *(.comment)
        *(.ARM.attributes)
    }
}
