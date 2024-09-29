; Stage 2 Bootloader

org 0x0
bits 16

%define CR 0x0D
%define LF 0x0A
%define ENDL CR, LF

%define KERNEL_LOAD_SEGMENT 0x3000
%define KERNEL_LOAD_OFFSET 0x0000

start:
    ; Print loading message
    mov si, msg_loaded_boot2
    call puts

    ; Save boot drive
    mov [ebr_drive_number], dl

    push es
    mov ah, 08h
    mov dl, [ebr_drive_number]
    int 13h
    jc disk_error
    pop es

    and cl, 0x3F                        ; remove top 2 bits
    xor ch, ch
    mov [bdb_sectors_per_track], cx     ; save sector count

    inc dh
    mov [bdb_heads], dh                 ; Save head count

    ; Set up segments
    mov ax, cs
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Set up stack
    mov ss, ax
    mov sp, 0xFFFF

    ; Print loading message
    mov si, msg_loading_kernel
    call puts

    ; Load kernel
    call load_kernel

    jmp $

    ; Switch to protected mode
    call switch_to_pm

    ; (hopefully) unreachable
    jmp $

load_kernel:
    ; compute LBA of root directory = reserved + fats * sectors_per_fat
    mov ax, [bdb_sectors_per_fat]
    mov bl, [bdb_fat_count]
    xor bh, bh
    mul bx                              ; ax = (fats * sectors_per_fat)
    add ax, [bdb_reserved_sectors]      ; ax = LBA of root directory
    push ax

    ; compute size of root directory = (32 * number_of_entries) / bytes_per_sector
    mov ax, [bdb_dir_entries_count]
    shl ax, 5                           ; ax *= 32
    xor dx, dx                          ; dx = 0
    div word [bdb_bytes_per_sector]     ; number of sectors we need to read

    test dx, dx                         ; if dx != 0, add 1
    jz .root_dir_after
    inc ax                              ; division remainder != 0, add 1
                                        ; this means we have a sector only partially filled with entries
.root_dir_after:
    ; read root directory
    mov cl, al                          ; cl = number of sectors to read = size of root directory
    pop ax                              ; ax = LBA of root directory
    mov dl, [ebr_drive_number]          ; dl = drive number (we saved it previously)
    mov bx, buffer                      ; es:bx = buffer
    call disk_read

    ; Find kernel.bin
    xor bx, bx
    mov di, buffer

.find_kernel:
    mov si, file_kernel_bin
    mov cx, 11                          ; compare 11 bytes (characters)
    push di
    repe cmpsb
    pop di
    je .found_kernel
    add di, 32
    inc bx
    cmp bx, [bdb_dir_entries_count]
    jl .find_kernel

    ; Kernel not found
    mov si, msg_kernel_not_found
    call puts
    jmp $

.found_kernel:
    push si
    mov si, msg_kernel_was_found
    call puts
    pop si

    ; di should have the address to the entry
    mov ax, [di + 26]                   ; First logical cluster field (offset 26)
    mov [kernel_cluster], ax

    ; Load FAT
    mov ax, [bdb_reserved_sectors]
    mov bx, buffer
    mov cl, [bdb_sectors_per_fat]
    mov dl, [ebr_drive_number]
    call disk_read

    ; Read kernel and process FAT chain
    mov bx, KERNEL_LOAD_SEGMENT
    mov es, bx
    mov bx, KERNEL_LOAD_OFFSET

.load_kernel_loop:
    ; Read next cluster
    mov ax, [kernel_cluster]
    add ax, 31                          ; first cluster = (kernel_cluster - 2) * sectors_per_cluster + start_sector
                                        ; start sector = reserved + fats + root directory size
    mov cl, 1
    mov dl, [ebr_drive_number]
    call disk_read

    push bx
    mov cx, [bdb_bytes_per_sector]
    mov si, bx
    call hex_dump
    pop bx

    add bx, [bdb_bytes_per_sector]

    ; Compute location of next cluster
    mov ax, [kernel_cluster]
    mov cx, 3
    mul cx
    mov cx, 2
    div cx                              ; ax = index of entry in FAT, dx = cluster mod 2

    mov si, buffer
    add si, ax
    mov ax, [ds:si]                     ; read entry from FAT table at index ax

    or dx, dx
    jz .even

.odd:
    shr ax, 4
    jmp .next_cluster_after

.even:
    and ax, 0x0FFF

.next_cluster_after:
    cmp ax, 0x0FF8                      ; end of chain
    jae .read_finish

    mov [kernel_cluster], ax
    jmp .load_kernel_loop

.read_finish:
    push si
    mov si, msg_loaded_kernel
    call puts
    pop si

    ret

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
    int 13h
    jc disk_error
    popa
    ret

disk_error:
    mov si, msg_read_failed
    call puts
    jmp $

;
; Dumps memory in hex format
;
hex_dump:
    push ax
    push bx
    push dx

.loop:
    lodsb
    mov ah, al
    shr al, 4
    call .print_hex_digit
    mov al, ah
    and al, 0x0F
    call .print_hex_digit
    mov al, ' '
    call .print_char
    loop .loop

    mov al, CR
    call .print_char
    mov al, LF
    call .print_char

    pop dx
    pop bx
    pop ax
    ret

.print_hex_digit:
    cmp al, 10
    jl .print_number
    add al, 'A' - 10
    jmp .print_char

.print_number:
    add al, '0'

.print_char:
    mov ah, 0x0E
    mov bh, 0
    int 0x10
    ret

; GDT ----------------------------------------------------------------

switch_to_pm:
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or al, 1
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

    in al, 0x92
    or al, 2
    out 0x92, al

    ; Jump to kernel
    call KERNEL_LOAD_SEGMENT:KERNEL_LOAD_OFFSET

    jmp $

gdt_start:
    dd 0x00000000
    dd 0x00000000
gdt_code:
    dw 0xffff
    dw 0x0000
    db 0x00
    db 10011010b    ; Access byte
    db 11001111b    ; Flags
    db 0x00
gdt_data:
    dw 0xffff
    dw 0x0000
    db 0x0
    db 10010010b    ; Access byte
    db 11001111b    ; Flags
    db 0x0
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

; Variables
msg_test:                db '[BOOT S2] DEBUG - GOT THIS FAR', 13, 10, 0
msg_loaded_boot2:        db '[BOOT S2] Loaded BOOT2.BIN', 13, 10, 0
msg_loading_kernel:      db '[BOOT S2] Loading kernel...', 13, 10, 0
msg_loaded_kernel:       db '[BOOT S2] Loaded KERNEL.BIN into memory', 13, 10, 0
msg_read_failed:         db "[BOOT S2] Disk Read Error", ENDL, 0
msg_kernel_was_found:    db '[BOOT S2] KERNEL.BIN was found!', 13, 10, 0
msg_kernel_not_found:    db '[BOOT S2] KERNEL.BIN not found!', 13, 10, 0
file_kernel_bin:         db 'KERNEL  BIN'
msg_disk_error:          db '[BOOT S2] Disk read error!', ENDL, 0

; BPB variables
kernel_cluster:          dw 0
bdb_bytes_per_sector:    dw 512
bdb_reserved_sectors:    dw 1
bdb_fat_count:           db 2
bdb_dir_entries_count:   dw 0E0h
bdb_sectors_per_fat:     dw 9
bdb_sectors_per_track:   dw 18
bdb_heads:               dw 2

ebr_drive_number:           db 0x00                 ; 0x00 floppy, 0x80 hdd
                            db 0                    ; reserved

buffer:                  times 512 db 0
root_dir_buffer:         times 512 db 0