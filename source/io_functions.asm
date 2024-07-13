; io_functions.asm

section .text

global inb
global outb
global inw
global outw

; Read a byte from a port
inb:
    mov dx, [esp + 4]       ; Load port number from stack
    in al, dx               ; Input byte from port into AL
    ret

; Write a byte to a port
outb:
    mov dx, [esp + 4]       ; Load port number from stack
    mov al, [esp + 8]       ; Load value to output from stack
    out dx, al              ; Output the value to the port
    ret

; Read a word (2 bytes) from a port
inw:
    mov dx, [esp + 4]       ; Load port number from stack
    in ax, dx               ; Input word from port into AX
    ret

; Write a word (2 bytes) from a port
outw:
    mov dx, [esp + 4]       ; Load port number from stack
    mov ax, [esp + 8]       ; Load word value to output from stack
    out dx, ax              ; Output the word value to the port
    ret
