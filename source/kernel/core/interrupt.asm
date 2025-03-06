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

; IRQ0 is the PIT
irq0:
    pushad
    cld
    call timer_handler
    popad
    iretd

; IRQ1 is the keyboard
irq1:
    pushad
    cld
    call keyboard_handler
    popad
    iretd

; IRQ6 is the floppy drive
irq6:
    pushad
    cld
    call floppy_handler
    popad
    iretd
