BITS 16
org 0x7C00

%define ENDL 0x0D, 0x0A

;
; FAT12 header
; 
jmp short start
nop

bdb_oem:                    db 'MSWIN4.1'           ; 8 bytes
bdb_bytes_per_sector:       dw 512
bdb_sectors_per_cluster:    db 1
bdb_reserved_sectors:       dw 1
bdb_fat_count:              db 2
bdb_dir_entries_count:      dw 0E0h
bdb_total_sectors:          dw 2880                 ; 2880 * 512 = 1.44MB
bdb_media_descriptor_type:  db 0F8h                 ; F8 = Hard disk
bdb_sectors_per_fat:        dw 9                    ; 9 sectors/fat
bdb_sectors_per_track:      dw 18
bdb_heads:                  dw 2
bdb_hidden_sectors:         dd 0
bdb_large_sector_count:     dd 0

; extended boot record
ebr_drive_number:           db 0                    ; 0x00 floppy, 0x80 hdd, useless
                            db 0                    ; reserved
ebr_signature:              db 29h
ebr_volume_id:              db 12h, 34h, 56h, 78h   ; serial number, value doesn't matter
ebr_volume_label:           db 'WAFFLE OS  '        ; 11 bytes, padded with spaces
ebr_system_id:              db 'FAT12   '           ; 8 bytes


start:
    mov ax, 0           ; can't set ds/es directly
    mov ds, ax
    mov es, ax
    
    ; setup stack
    mov ss, ax
    mov sp, 0x7C00              ; stack grows downwards from where we are loaded in memory

    mov si, MSG_REAL_MODE
    call print_string

    call load_kernel

    mov si, MSG_LOAD_KERNEL
    call print_string

    call switch_to_pm           ; switch to protected mode
    jmp $                       ; hang (should be unreachable)

load_kernel:
    mov bx, KERNEL_OFFSET
    mov dh, 35                  ; load 35 sectors
    mov dl, [BOOT_DISK]         ; load from boot disk
    call disk_load
    ret

disk_load:
    push dx
    mov ah, 0x02                ; BIOS read sector
    mov al, dh
    mov ch, 0x00                ; cylinder
    mov dh, 0x00                ; head
    mov cl, 0x02                ; start reading second sector
    int 0x13
    jc disk_error
    pop dx
    cmp dh, al
    jne disk_error              ; print err message if something goes wrong
    ret

disk_error:
    mov si, DISK_ERROR_MSG
    call print_string
    jmp $                       ; hang

print_string:
    mov ah, 0x0e                ; print char fn
.loop:
    lodsb
    cmp al, 0                   ; check for string termination
    je .done
    int 0x10
    jmp .loop
.done:
    ret

switch_to_pm:                   ; switch to protected mode
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 0x1                 ; protected mode bit
    mov cr0, eax
    jmp CODE_SEG:init_pm        ; jump to 32 bit movde

; 32 bit protected mode ------------------------------------------------------------------------
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

    jmp $                    ; hang (should be unreachable)

gdt_start:
    dd 0x0
    dd 0x0
gdt_code:
    dw 0xffff
    dw 0x0000
    db 0x00
    db 10011010b
    db 11001111b
    db 0x00
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

MSG_REAL_MODE db "[BOOT] Started in 16-bit Real Mode", 13, 10, 0
MSG_LOAD_KERNEL db "[BOOT] Loading kernel into memory", 13, 10, 0
DISK_ERROR_MSG db "[BOOT] Disk read error!", 13, 10, 0

times 510-($-$$) db 0
dw 0xAA55