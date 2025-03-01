; Kernel x86 helper functions
; x86_inb and x86_outb source: https://github.com/nanobyte-dev/nanobyte_os/blob/videos/part7/src/kernel/x86.asm

; Read a byte from a port
global x86_inb
x86_inb:
    [bits 32]
    mov dx, [esp + 4]   ; Load port number from stack
    xor eax, eax
    in al, dx           ; Input byte from port into AL
    ret

; Write a byte to a port
global x86_outb
x86_outb:
    [bits 32]
    mov dx, [esp + 4]   ; Load port number from stack
    mov al, [esp + 8]   ; Load value to output from stack
    out dx, al          ; Output the value to the port
    ret

; Read a word from a port
global x86_inw
x86_inw:
    [bits 32]
    xor eax, eax         ; Clear entire EAX register
    mov dx, [esp + 4]    ; Load port number from stack
    in ax, dx           ; Input word from port into AX (upper 16 bits remain 0)
    ret

; Write a word to a port
global x86_outw
x86_outw:
    [bits 32]
    mov dx, [esp + 4]   ; Load port number from stack
    mov ax, [esp + 8]   ; Load word value to output from stack
    out dx, ax          ; Output the word value to the port
    ret


