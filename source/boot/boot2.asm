[ORG 0]
[BITS 16]

; NOTE: A Significant portion of this file was derived from the work of Nanobyte (https://github.com/nanobyte-dev/nanobyte_os/blob/videos/part3/src/bootloader/boot.asm)

%define ENDL 0x0D, 0x0A

jmp short start
nop

bdb_oem:                    db 'mkfs.fat'           ; 8 bytes
bdb_bytes_per_sector:       dw 512
bdb_sectors_per_cluster:    db 1
bdb_reserved_sectors:       dw 1
bdb_fat_count:              db 2
bdb_dir_entries_count:      dw 0E0h
bdb_total_sectors:          dw 2880                 ; 1.44MB floppy
bdb_media_descriptor_type:  db 0F0h                 ; F0h = floppy
bdb_sectors_per_fat:        dw 9
bdb_sectors_per_track:      dw 18
bdb_heads:                  dw 2
bdb_hidden_sectors:         dd 0
bdb_large_sector_count:     dd 0

; EBR fields:
ebr_drive_number:           db 0                    ; will be set from DL
                            db 0
ebr_signature:              db 29h
ebr_volume_id:              db 12h,34h,56h,78h
ebr_volume_label:           db 'WAFFLE   OS'
ebr_system_id:              db 'FAT12   '

file_kernel_bin   db 'KERNEL  BIN' ; 8.3 filename
kernel_cluster    dw 0

; Constants for kernel load location:
KERNEL_LOAD_SEGMENT    equ 0x3000
KERNEL_LOAD_OFFSET     equ 0x0000

start:
    cld

    mov ax, 0x2000
    mov ds, ax
    mov es, ax

    ; Assume the boot drive remains in DL from stage 1.
    mov [ebr_drive_number], dl

    mov si, MSG_REAL_MODE
    call puts

    ; --- Read drive parameters via BIOS ---
    push es
    mov ah, 08h
    int 13h
    jc floppy_error
    pop es

    and cl, 0x3F         ; sectors-per-track (clear top 2 bits)
    xor ch, ch
    mov [bdb_sectors_per_track], cx

    inc dh
    mov [bdb_heads], dh

    ; --- Compute LBA of root directory ---
    mov ax, [bdb_sectors_per_fat]
    mov bl, [bdb_fat_count]
    xor bh, bh
    mul bx               ; AX = (FAT count * sectors_per_fat)
    add ax, [bdb_reserved_sectors]
    push ax

    ; --- Compute number of sectors in root directory ---
    mov ax, [bdb_dir_entries_count]
    shl ax, 5            ; (entry size is 32 bytes)
    xor dx, dx
    div word [bdb_bytes_per_sector]

    test dx, dx
    jz .root_dir_after
    inc ax

.root_dir_after:
    ; --- Read root directory into buffer ---
    mov cl, al         ; number of sectors to read
    pop ax             ; AX = LBA of root directory
    mov dl, [ebr_drive_number]
    mov bx, buffer     ; Buffer (defined at end of file)
    call disk_read

    ; --- Search for "KERNEL  BIN" in the root directory ---
    xor bx, bx
    mov di, buffer
.search_kernel:
    mov si, file_kernel_bin
    mov cx, 11
    push di
    repe cmpsb
    pop di
    je .found_kernel

    add di, 32         ; next directory entry
    inc bx
    cmp bx, [bdb_dir_entries_count]
    jl .search_kernel

    jmp kernel_not_found_error

.found_kernel:
    mov ax, [di+26]    ; first logical cluster from directory entry
    mov [kernel_cluster], ax

    ; read fat table from disk into the buffer
    mov ax, [bdb_reserved_sectors]
    mov bx, buffer
    mov cl, [bdb_sectors_per_fat]
    mov dl, [ebr_drive_number]
    call disk_read

    mov si, MSG_LOAD_KERNEL
    call puts

    ; follow fat chain
    mov ax, KERNEL_LOAD_SEGMENT
    mov es, ax
    mov bx, KERNEL_LOAD_OFFSET

.load_kernel_loop:
    mov ax, [kernel_cluster]
    add ax, 31         ; (hardcoded: first cluster begins at LBA 33)
    mov cl, 1          ; one sector per cluster
    mov dl, [ebr_drive_number]
    call disk_read
    add bx, [bdb_bytes_per_sector]

    ; determine next cluster from FAT
    mov ax, [kernel_cluster]
    mov cx, 3
    mul cx
    mov cx, 2
    div cx             ; AX now holds offset into FAT table
    mov si, buffer
    add si, ax
    mov ax, [ds:si]
    or dx, dx
    jz .even
.odd:
    shr ax, 4
    jmp .next_cluster_after
.even:
    and ax, 0x0FFF
.next_cluster_after:
    cmp ax, 0x0FF8    ; end-of-chain marker in FAT12
    jae .read_finish
    mov [kernel_cluster], ax
    jmp .load_kernel_loop
.read_finish:
    call switch_to_pm
    jmp $

floppy_error:
    mov si, DISK_ERROR_MSG
    call puts
    jmp wait_key_and_reboot

kernel_not_found_error:
    mov si, msg_kernel_not_found
    call puts
    jmp wait_key_and_reboot

wait_key_and_reboot:
    mov ah, 0
    int 16h
    jmp 0xFFFF:0

; Routine: puts - Nanobyte
puts:
    push si
.loop_puts:
    lodsb
    or al, al
    jz .done_puts
    mov ah, 0x0E
    int 10h
    jmp .loop_puts
.done_puts:
    pop si
    ret


; Routine: lba_to_chs - Nanobyte
lba_to_chs:
    push ax
    push dx
    xor dx, dx
    div word [bdb_sectors_per_track]    ; AX = LBA / sectors-per-track, DX = remainder
    inc dx                              ; sector = remainder + 1
    mov cx, dx                          ; store sector in lower bits of CX
    xor dx, dx
    div word [bdb_heads]                ; AX = cylinder, DX = head
    mov dh, dl                        ; head in DH
    mov ch, al                        ; cylinder low byte in CH
    shl ah, 6
    or cl, ah                         ; merge upper 2 bits of cylinder into CL
    pop dx
    pop ax
    ret

; Routine: disk_read - Nanobyte
disk_read:
    push ax
    push bx
    push cx
    push dx
    push di
    push cx                ; save number of sectors to read
    call lba_to_chs
    pop ax                 ; restore sector count into AL
    mov ah, 02h            ; BIOS read sectors function
    mov di, 3              ; retry count
.retry_disk_read:
    pusha
    stc                    ; set carry flag (some BIOSes require it)
    int 13h
    jnc .done_disk_read
    popa
    call disk_reset
    dec di
    test di, di
    jnz .retry_disk_read
.fail_disk_read:
    jmp floppy_error
.done_disk_read:
    popa
    pop di
    pop dx
    pop cx
    pop bx
    pop ax
    ret

disk_reset:
    pusha
    mov ah, 0
    stc
    int 13h
    jc floppy_error
    popa
    ret

switch_to_pm:
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 0x1             ; set PE bit
    mov cr0, eax
    jmp CODE_SEG:init_pm

; 32-bit Protected-mode entry
[BITS 32]
init_pm:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ebp, 0x90000
    mov esp, ebp
    call KERNEL_LOAD_SEGMENT:KERNEL_LOAD_OFFSET
    jmp $

; GDT
[BITS 16]
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
    dw 0x0000
    db 0x00
    db 10010010b
    db 11001111b
    db 0x00
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

; Message strings
MSG_REAL_MODE     db "[BOOT2] Stage 2: Real Mode", ENDL,0
MSG_LOAD_KERNEL   db "[BOOT2] Loading kernel into memory", ENDL,0
DISK_ERROR_MSG    db "[BOOT2] Disk read error!", ENDL,0
msg_kernel_not_found db "[BOOT2] KERNEL.BIN file not found!", ENDL,0

buffer: