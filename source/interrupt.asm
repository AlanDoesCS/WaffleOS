global load_idt
global irq1
extern keyboard_handler

section .text

load_idt:
    mov edx, [esp + 4]
    lidt [edx]
    ret

irq1:
    pushad
    cld
    call keyboard_handler
    popad
    iretd