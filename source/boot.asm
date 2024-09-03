; Derived from https://github.com/nanobyte-dev/nanobyte_os/blob/videos/part3/src/bootloader/boot.asm

; Stage 1 bootloader

org 0x7C00
bits 16

%define ENDL 0x0D, 0x0A

; FAT12 header ---------------------------------------------------------------------------------------------------------

jmp short start
nop

bdb_oem:                    db 'MSWIN4.1'           ; 8 bytes
bdb_bytes_per_sector:       dw 512
bdb_sectors_per_cluster:    db 1
bdb_reserved_sectors:       dw 1
bdb_fat_count:              db 2
bdb_dir_entries_count:      dw 0E0h
bdb_total_sectors:          dw 2880                 ; 2880 * 512 = 1.44MB
bdb_media_descriptor_type:  db 0F0h
bdb_sectors_per_fat:        dw 9
bdb_sectors_per_track:      dw 18
bdb_heads:                  dw 2
bdb_hidden_sectors:         dd 0
bdb_large_sector_count:     dd 0

; extended boot record
ebr_drive_number:           db 0x00                 ; 0x00 floppy, 0x80 hdd
                            db 0                    ; reserved
ebr_signature:              db 29h
ebr_volume_id:              db 12h, 34h, 56h, 78h   ; Serial Num
ebr_volume_label:           db 'WAFFLE OS  '        ; 11 bytes, padded with spaces
ebr_system_id:              db 'FAT12   '           ; 8 bytes

; start of code --------------------------------------------------------------------------------------------------------

start:
    xor ax, ax
    mov ds, ax
    mov es, ax
    
    ; init stack segment
    mov ss, ax
    mov sp, 0x7C00

    ; save boot drive number
    mov [ebr_drive_number], dl

    ; show loading message
    mov si, msg_loading_stage2
    call puts

    mov ah, 08h
    mov dl, [ebr_drive_number]
    int 13h
    jc disk_error

    and cl, 0x3F                        ; remove top 2 bits
    xor ch, ch
    mov [bdb_sectors_per_track], cx     ; save sector count

    inc dh
    mov [bdb_heads], dh                 ; Save head count

    ; compute LBA of root directory = reserved + fats * sectors_per_fat
    ; note: this section can be hardcoded
    mov ax, [bdb_sectors_per_fat]
    movzx bx, byte [bdb_fat_count]
    mul bx
    add ax, [bdb_reserved_sectors]
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

    ; Find boot2.bin
    mov cx, [bdb_dir_entries_count]
    mov di, buffer

.find_stage2:
    push cx
    mov cx, 11                          ; compare 11 bytes (characters)
    mov si, file_stage2_bin
    push di
    repe cmpsb
    pop di
    je .found_stage2
    pop cx
    add di, 32
    loop .find_stage2

    ; Stage2 not found
    mov si, msg_stage2_not_found
    call puts
    jmp $

.found_stage2:
    ; di should have the address to the entry
    mov ax, [di + 26]                   ; First logical cluster field (offset 26)
    mov [stage2_cluster], ax

    ; Load FAT
    mov ax, [bdb_reserved_sectors]
    mov bx, buffer
    mov cl, [bdb_sectors_per_fat]
    mov dl, [ebr_drive_number]
    call disk_read

    ; Read stage2 and process FAT chain
    mov bx, 0x7E00                      ; Load stage2 right after bootloader

.load_stage2_loop:
    ; Read next cluster
    mov ax, [stage2_cluster]
    sub ax, 2
    movzx cx, byte [bdb_sectors_per_cluster]
    mul cx
    mov cx, ax

    mov ax, [bdb_reserved_sectors]
    add ax, [bdb_sectors_per_fat]
    movzx dx, byte [bdb_fat_count]
    mul dx
    add ax, cx

    mov cl, [bdb_sectors_per_cluster]
    mov dl, [ebr_drive_number]
    call disk_read

    add bx, [bdb_bytes_per_sector]

    ; compute location of next cluster
    mov ax, [stage2_cluster]
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

    mov [stage2_cluster], ax
    jmp .load_stage2_loop

.read_finish:

    ; jump to stage 2
    mov dl, [ebr_drive_number]          ; boot device in dl
    jmp 0x7E00

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

disk_error:
    mov si, msg_read_failed
    call puts
    jmp $

msg_loading_stage2:     db "BOOT->BOOT2", ENDL, 0 ; loading stage2 message
msg_read_failed:        db "Rd Err", ENDL, 0
msg_stage2_not_found:   db "BOOT2 not found!", ENDL, 0
file_stage2_bin:        db "BOOT2   BIN"
stage2_cluster:         dw 0

buffer:

times 510-($-$$) db 0
dw 0AA55h
