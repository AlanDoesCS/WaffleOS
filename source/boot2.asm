; Stage 2 Bootloader

org 0x7E00
bits 16

%define ENDL 0x0D, 0x0A

%define KERNEL_LOAD_SEGMENT 0x2000
%define KERNEL_LOAD_OFFSET 0x0000

start:
    ; Set up segments
    mov ax, cs
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Set up stack
    mov ss, ax
    mov sp, 0x7C00

    ; Save boot drive
    mov [boot_drive], dl

    ; Print loading message
    mov si, msg_loading_kernel
    call puts

    ; Load kernel
    call load_kernel

    ; Switch to protected mode
    call switch_to_pm

    ; (hopefully) unreachable
    jmp $

load_kernel:
    ; Load root directory
    mov ax, [bdb_reserved_sectors]
    add ax, [bdb_sectors_per_fat]
    add ax, [bdb_sectors_per_fat]  ; AX = LBA of root directory
    mov cx, [bdb_dir_entries_count]
    shr cx, 4  ; CX = Number of sectors to read (each sector holds 16 entries)

    mov bx, buffer
    call disk_read

    ; Find kernel.bin
    mov di, buffer
    mov cx, [bdb_dir_entries_count]

.find_kernel:
    push cx
    mov cx, 11
    mov si, file_kernel_bin
    push di
    repe cmpsb
    pop di
    je .found_kernel
    pop cx
    add di, 32
    loop .find_kernel

    ; Kernel not found
    mov si, msg_kernel_not_found
    call puts
    jmp $

.found_kernel:
    ; Load kernel.bin
    mov ax, [di + 26]  ; First cluster
    mov [kernel_cluster], ax

    ; Load FAT
    mov ax, [bdb_reserved_sectors]
    mov bx, buffer
    mov cx, [bdb_sectors_per_fat]
    call disk_read

    ; Load kernel
    mov ax, KERNEL_LOAD_SEGMENT
    mov es, ax
    mov bx, KERNEL_LOAD_OFFSET

.load_kernel_loop:
    ; Read next cluster
    mov ax, [kernel_cluster]
    add ax, 31  ; Convert cluster number to LBA
    mov cl, 1
    call disk_read

    add bx, 512
    jc .next_segment

    jmp .find_next_cluster

.next_segment:
    push ax
    mov ax, es
    add ax, 0x1000
    mov es, ax
    pop ax
    xor bx, bx

.find_next_cluster:
    ; Find next cluster in FAT
    mov ax, [kernel_cluster]
    mov cx, 3
    mul cx
    mov cx, 2
    div cx  ; AX = index of entry in FAT, DX = cluster mod 2

    mov si, buffer
    add si, ax
    mov ax, [si]

    or dx, dx
    jz .even
.odd:
    shr ax, 4
    jmp .next_cluster_after
.even:
    and ax, 0x0FFF

.next_cluster_after:
    cmp ax, 0x0FF8  ; End of chain?
    jae .kernel_loaded

    mov [kernel_cluster], ax
    jmp .load_kernel_loop

.kernel_loaded:
    ret

switch_to_pm:
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp CODE_SEG:init_pm

bits 32
init_pm:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ebp, 0x90000
    mov esp, ebp

    ; Jump to kernel
    jmp KERNEL_LOAD_SEGMENT:KERNEL_LOAD_OFFSET

bits 16
;
; Prints a string to the screen
; Params:
;   - ds:si points to string
;
puts:
    ; save registers we will modify
    push si
    push ax
    push bx

.loop:
    lodsb               ; loads next character in al
    or al, al           ; verify if next character is null?
    jz .done

    mov ah, 0x0E        ; call bios interrupt
    mov bh, 0           ; set page number to 0
    int 0x10

    jmp .loop

.done:
    pop bx
    pop ax
    pop si
    ret

;
; Disk routines
;

;
; Converts an LBA address to a CHS address
; Parameters:
;   - ax: LBA address
; Returns:
;   - cx [bits 0-5]: sector number
;   - cx [bits 6-15]: cylinder
;   - dh: head
;

lba_to_chs:

    push ax
    push dx

    xor dx, dx                          ; dx = 0
    div word [bdb_sectors_per_track]    ; ax = LBA / SectorsPerTrack
                                        ; dx = LBA % SectorsPerTrack

    inc dx                              ; dx = (LBA % SectorsPerTrack + 1) = sector
    mov cx, dx                          ; cx = sector

    xor dx, dx                          ; dx = 0
    div word [bdb_heads]                ; ax = (LBA / SectorsPerTrack) / Heads = cylinder
                                        ; dx = (LBA / SectorsPerTrack) % Heads = head
    mov dh, dl                          ; dh = head
    mov ch, al                          ; ch = cylinder (lower 8 bits)
    shl ah, 6
    or cl, ah                           ; put upper 2 bits of cylinder in CL

    pop ax
    mov dl, al                          ; restore DL
    pop ax
    ret


;
; Reads sectors from a disk
; Parameters:
;   - ax: LBA address
;   - cl: number of sectors to read (up to 128)
;   - dl: drive number
;   - es:bx: memory address where to store read data
;
disk_read:

    push ax                             ; save registers we will modify
    push bx
    push cx
    push dx
    push di

    push cx                             ; temporarily save CL (number of sectors to read)
    call lba_to_chs                     ; compute CHS
    pop ax                              ; AL = number of sectors to read

    mov ah, 02h
    mov di, 3                           ; retry count

.retry:
    pusha                               ; save all registers, we don't know what bios modifies
    stc                                 ; set carry flag, some BIOS'es don't set it
    int 13h                             ; carry flag cleared = success
    jnc .done                           ; jump if carry not set

    ; read failed
    popa
    call disk_reset

    dec di
    test di, di
    jnz .retry

.fail:
    ; all attempts are exhausted
    jmp disk_error

.done:
    popa

    pop di
    pop dx
    pop cx
    pop bx
    pop ax                             ; restore registers modified
    ret

;
; Resets disk controller
; Parameters:
;   dl: drive number
;

disk_reset:
    pusha
    mov ah, 0
    stc
    int 13h
    jc disk_error
    popa
    ret

; GDT
gdt_start:
    dq 0x0
gdt_code:
    dw 0xFFFF
    dw 0x0
    db 0x0
    db 10011010b
    db 11001111b
    db 0x0
gdt_data:
    dw 0xFFFF
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

; Variables
boot_drive:          db 0
kernel_cluster:      dw 0
msg_loading_kernel:  db 'Loading kernel...', 13, 10, 0
msg_kernel_not_found: db 'KERNEL.BIN not found!', 13, 10, 0
file_kernel_bin:     db 'KERNEL  BIN'

; Include necessary data from FAT12 header
; fixed value :( big sad
bdb_reserved_sectors:    dw 1
bdb_sectors_per_fat:     dw 9
bdb_dir_entries_count:   dw 0E0h

buffer: