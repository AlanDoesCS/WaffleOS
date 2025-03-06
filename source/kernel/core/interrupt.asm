[BITS 32]

global i686_IDT_Load
global irq0
extern timer_handler
global irq1
extern keyboard_handler
global irq6
extern floppy_handler
global irq12
extern mouse_handler

section .text

; Start of code copied from: https://github.com/nanobyte-dev/nanobyte_os/blob/videos/part8/src/kernel/arch/i686/idt.asm
; void __attribute__((cdecl)) i686_IDT_Load(IDTDescriptor* idtDescriptor);
i686_IDT_Load:
    ; make new call frame
    push ebp             ; save old call frame
    mov ebp, esp         ; initialize new call frame

    ; load idt
    mov eax, [ebp + 8]
    lidt [eax]

    ; restore old call frame
    mov esp, ebp
    pop ebp
    ret
; End of code from nanobyte

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

; IRQ12 is the PS/2 mouse
irq12:
    pushad
    cld
    call mouse_handler
    popad
    iretd