BITS 16
org 0x7C00

KERNEL_OFFSET equ 0x1000
BOOT_DISK: db 0

start:
    mov [BOOT_DISK], dl

    mov si, MSG_REAL_MODE
    call print_string

    call load_kernel

    mov si, MSG_LOAD_KERNEL
    call print_string

    call switch_to_pm
    jmp $

load_kernel:
    mov bx, KERNEL_OFFSET
    mov dh, 15
    mov dl, [BOOT_DISK]
    call disk_load
    ret

disk_load:
    push dx
    mov ah, 0x02
    mov al, dh
    mov ch, 0x00
    mov dh, 0x00
    mov cl, 0x02
    int 0x13
    jc disk_error
    pop dx
    cmp dh, al
    jne disk_error
    ret

disk_error:
    mov si, DISK_ERROR_MSG
    call print_string
    jmp $

print_string:
    mov ah, 0x0e
.loop:
    lodsb
    cmp al, 0
    je .done
    int 0x10
    jmp .loop
.done:
    ret

switch_to_pm:
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp CODE_SEG:init_pm

BITS 32
init_pm:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ebp, 0x90000
    mov esp, ebp

    call KERNEL_OFFSET

    jmp $

gdt_start:
    dq 0x0
gdt_code:
    dw 0xffff
    dw 0x0
    db 0x0
    db 10011010b
    db 11001111b
    db 0x0
gdt_data:
    dw 0xffff
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

MSG_REAL_MODE db "Started in 16-bit Real Mode", 13, 10, 0
MSG_LOAD_KERNEL db "Loading kernel into memory", 13, 10, 0
DISK_ERROR_MSG db "Disk read error!", 13, 10, 0

times 510-($-$$) db 0
dw 0xaa55