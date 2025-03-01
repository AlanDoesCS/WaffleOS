// main code for stage2 bootloader
// based on: https://github.com/nanobyte-dev/nanobyte_os/blob/videos/part7/src/bootloader/stage2/main.c
#include <stdint.h>
#include "stdio.h"
#include "x86.h"
#include "disk.h"
#include "fat.h"
#include "memdefs.h"
#include "memory.h"
#include "vesa.h"

extern uint32_t vbe_lfb_address;

typedef struct BootInfo {
    uint16_t boot_drive;
    uint32_t lfb_address;
} BootInfo;

typedef void (*KernelStart)(BootInfo*);

uint8_t* KernelLoadBuffer = (uint8_t*)MEMORY_LOAD_KERNEL;
uint8_t* Kernel = (uint8_t*)MEMORY_KERNEL_ADDR;

void __attribute__((cdecl)) start(uint16_t bootDrive)
{
    clrscr();

    DISK disk;
    if (!DISK_Initialize(&disk, bootDrive))
    {
        printf("Disk init error\r\n");
        goto end;
    }

    if (!FAT_Initialize(&disk))
    {
        printf("FAT init error\r\n");
        goto end;
    }
    set_vesa_mode();

    // load kernel
    FAT_File* fd = FAT_Open(&disk, "/kernel.bin");
    uint32_t read;
    uint8_t* kernelBuffer = Kernel;
    while ((read = FAT_Read(&disk, fd, MEMORY_LOAD_SIZE, KernelLoadBuffer)))
    {
        memcpy(kernelBuffer, KernelLoadBuffer, read);
        kernelBuffer += read;
    }
    FAT_Close(fd);

    BootInfo boot_info = {
        .boot_drive = bootDrive,
        .lfb_address = vbe_lfb_address
    };

    // execute kernel
    KernelStart kernelStart = (KernelStart)Kernel;
    kernelStart(&boot_info);

    end:
        for (;;);
}
