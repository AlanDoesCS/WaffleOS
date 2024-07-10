BITS 16
org 0x7C00

jmp $

times 510-($-$$) db 0   ; Make total bootloader size equal to 510 bytes
dw 0xAA55               ; Boot signature (+2 bytes)