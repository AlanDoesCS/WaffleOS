//
// Created by Alan on 12/07/2024.
// ATA PIO mode driver based on : https://wiki.osdev.org/ATA_PIO_Mode
//

#include "disk.h"
#include "display.h"

#define ATA_PRIMARY_DATA_PORT 0x1F0
#define ATA_PRIMARY_ERROR_PORT 0x1F1
#define ATA_PRIMARY_SECTOR_COUNT_PORT 0x1F2
#define ATA_PRIMARY_LBA_LOW_PORT 0x1F3
#define ATA_PRIMARY_LBA_MID_PORT 0x1F4
#define ATA_PRIMARY_LBA_HIGH_PORT 0x1F5
#define ATA_PRIMARY_DRIVE_PORT 0x1F6
#define ATA_PRIMARY_COMMAND_PORT 0x1F7

#define ATA_IDENTIFY_DRIVE 0xEC

#define ATA_SECONDARY_DATA_PORT 0x170
#define ATA_SECONDARY_ERROR_PORT 0x171

#define ATA_MASTER_DRIVE 0xA0
#define ATA_SLAVE_DRIVE 0xB0

// Helper functions for reading/writing from I/O
extern unsigned char inb(unsigned short port);
extern void outb(unsigned short port, unsigned char val);

void init_disk(void) {
    println("[DISK] Disk initialized");
}

/*
void wait_400ns() {	// Less efficient: wastes clock cycles
    inb(ATA_PRIMARY_DATA_PORT + 0x00);
    inb(ATA_PRIMARY_DATA_PORT + 0x00);
    inb(ATA_PRIMARY_DATA_PORT + 0x00);
    inb(ATA_PRIMARY_DATA_PORT + 0x00);
}
*/