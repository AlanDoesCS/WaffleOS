BITS 16
org 0x7C00

start:
    mov ax, 0x0003      ; Text mode
    int 0x10            ; Interrupt which calls BIOS video services

    mov si, booting_str
    call print_string

    mov si, thanks_str
    call print_string

    jmp hang

print_string:           ; print string of characters
    lodsb               ; Load next character into al
    or al, al           ; Check if the char is the end of the string (zero)
    jz .end             ; end on null terminated string
    mov ah, 0x0E        ; BIOS teletype function for character printing
    int 0x10            ; call BIOS print
    jmp print_string
.end:
    ret

hang:
    cli
    hlt
    jmp hang

; strings:
booting_str db 'Booting WaffleOS...', 0
thanks_str db 13, 10, 13, 10, 'Thank you for using WaffleOS! :)', 0

times 510-($-$$) db 0   ; Make total bootloader size equal to 510 bytes
dw 0xAA55               ; Boot signature (+2 bytes)