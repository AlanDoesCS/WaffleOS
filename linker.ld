ENTRY(kernel_main)
SECTIONS
{
    . = 0x3000;
    .text : {
        *(.text.kernel_entry)
        *(.text)
    }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    .bss : { *(.bss) }
}