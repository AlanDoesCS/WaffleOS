; set_vesa_mode.asm
BITS 16

global set_vesa_mode

; Structure to hold VESA mode information
struc VbeModeInfoBlock
    .ModeAttributes      resw 1
    .WinAAttributes      resb 1
    .WinBAttributes      resb 1
    .WinGranularity      resw 1
    .WinSize             resw 1
    .WinASegment         resw 1
    .WinBSegment         resw 1
    .WinFuncPtr          resd 1
    .BytesPerScanLine    resw 1
    .XResolution         resw 1
    .YResolution         resw 1
    .XCharSize           resb 1
    .YCharSize           resb 1
    .NumberOfPlanes      resb 1
    .BitsPerPixel        resb 1
    .NumberOfBanks       resb 1
    .MemoryModel         resb 1
    .BankSize            resb 1
    .NumberOfImagePages  resb 1
    .Reserved            resb 1
    .RedMaskSize         resb 1
    .RedFieldPosition    resb 1
    .GreenMaskSize       resb 1
    .GreenFieldPosition  resb 1
    .BlueMaskSize        resb 1
    .BlueFieldPosition   resb 1
    .ReservedMaskSize    resb 1
    .ReservedFieldPosition resb 1
    .DirectColorModeInfo resb 1
    .PhysBasePtr         resd 1
    .OffScreenMemOffset  resd 1
    .OffScreenMemSize    resw 1
    .Reserved2           resb 206
endstruc

section .bss
vbeModeInfoBlock resb VbeModeInfoBlock_size

section .text

set_vesa_mode:
    pusha

    ; Set up the mode number (e.g., 0x118)
    mov ax, 0x4F02
    mov bx, 0112  ; Mode number (adjust as needed)
    int 0x10
    cmp ax, 0x004F
    jne .error

    ; Retrieve mode information
    mov ax, 0x4F01
    mov cx, 0x118  ; Mode number (same as above)
    mov di, vbeModeInfoBlock
    int 0x10
    cmp ax, 0x004F
    jne .error

    ; Successfully set mode
    popa
    ret

.error:
    ; Handle error (e.g., revert to text mode 0x03)
    mov ax, 0x0003
    int 0x10
    popa
    ret
