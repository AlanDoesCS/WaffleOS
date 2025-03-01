global load_idt
global irq0
extern timer_handler
global irq1
extern keyboard_handler
global irq6
extern floppy_handler

section .text

load_idt:
    mov edx, [esp + 4]
    lidt [edx]
    ret

irq0:
    pushad
    cld
    call timer_handler
    popad
    iretd

irq1:
    pushad
    cld
    call keyboard_handler
    popad
    iretd

irq6:
    pushad
    cld
    call keyboard_handler
    popad
    iretd