; io_functions.asm

section .text

global inb
global outb

; Read a byte from a port
inb:
    mov dx, [esp + 4]       ; Load port number from stack
    in al, dx               ; Input byte from port into AL
    ret                     ; Return from function

; Write a byte to a port
outb:
    mov dx, [esp + 4]       ; Load port number from stack
    mov al, [esp + 8]       ; Load value to output from stack
    out dx, al              ; Output the value to the port
    ret                     ; Return from function
