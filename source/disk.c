//
// Created by Alan on 12/07/2024.
// ATA PIO mode driver based on : https://wiki.osdev.org/ATA_PIO_Mode
//

#include "disk.h"
#include "display.h"
#include "types.h"

#define ATA_PRIMARY_DATA_PORT 0x1F0
#define ATA_PRIMARY_ERROR_PORT 0x1F1
#define ATA_PRIMARY_SECTOR_COUNT_PORT 0x1F2
#define ATA_PRIMARY_LBA_LOW_PORT 0x1F3
#define ATA_PRIMARY_LBA_MID_PORT 0x1F4
#define ATA_PRIMARY_LBA_HIGH_PORT 0x1F5
#define ATA_PRIMARY_DRIVE_PORT 0x1F6
#define ATA_PRIMARY_COMMAND_PORT 0x1F7
#define ATA_PRIMARY_STATUS_PORT 0x1F7

#define ATA_PRIMARY_ALT_STATUS_PORT 0x3F6
#define ATA_SECONDARY_ALT_STATUS_PORT 0x376

#define ATA_READ_SECTORS_CMD 0x20
#define ATA_WRITE_SECTORS_CMD 0x30

#define ATA_READ_MULTIPLE_CMD 0xC4
#define ATA_WRITE_MULTIPLE_CMD 0xC5

#define ATA_IDENTIFY_DRIVE_CMD 0xEC
#define ATA_FLUSH_CACHE_CMD 0xE7

#define ATA_SECONDARY_DATA_PORT 0x170
#define ATA_SECONDARY_ERROR_PORT 0x171

#define ATA_MASTER_DRIVE 0xA0
#define ATA_SLAVE_DRIVE 0xB0

// ATA status
#define ATA_STATUS_BSY 0x80		// Busy
#define ATA_STATUS_DRDY 0x40	// Device ready
#define ATA_STATUS_DRQ 0x08		// Data request
#define ATA_STATUS_ERR 0x01		// Error
#define ATA_STATUS_DF 0x20		// Device fault

#define ATA_FLOATING_BUS 0xFF

static uint8_t last_selected_device = ATA_FLOATING_BUS;  // initial value is invalid - No drive

// Helper functions for reading/writing from I/O
extern uint8_t inb(uint16_t port);
extern void outb(uint16_t port, uint8_t val);
extern uint16_t inw(uint16_t port);
extern void outw(uint16_t port, uint16_t val);

uint8_t read_status(void) {
    return inb(ATA_PRIMARY_ALT_STATUS_PORT);
}

char* read_status_str() {
    uint8_t status = read_status();
    if (status & ATA_STATUS_BSY) {
        return "BUSY";
    } else if (status & ATA_STATUS_DRDY) {
        return "READY";
    } else if (status & ATA_STATUS_DRQ) {
        return "DATA REQUEST";
    } else if (status & ATA_STATUS_ERR) {
        return "ERROR";
    } else if (status & ATA_STATUS_DF) {
        return "DEVICE FAULT";
    } else {
        return "UNKNOWN";
    }
}

void wait_not_busy(void) {
    while (read_status() & ATA_STATUS_BSY);
}

void wait_ready(void) {
    while (!(read_status() & ATA_STATUS_DRDY));
}

int identify_device(void) {
    select_device(ATA_MASTER_DRIVE);
    wait_not_busy();

    // Identify cmd params
    outb(ATA_PRIMARY_SECTOR_COUNT_PORT, 0);
    outb(ATA_PRIMARY_LBA_LOW_PORT, 0);
    outb(ATA_PRIMARY_LBA_MID_PORT, 0);
    outb(ATA_PRIMARY_LBA_HIGH_PORT, 0);

    outb(ATA_PRIMARY_COMMAND_PORT, ATA_IDENTIFY_DRIVE_CMD);

    // Read status until not busy and neither error or data request bits are set
    uint8_t status;
    do {
        status = read_status();
    } while (status & ATA_STATUS_BSY);

    if (status & ATA_STATUS_ERR) {
        println("[DISK] Error during device identification");
        return 0;
    }

    for (int i = 0; i < 256; i++) {
        uint16_t data = inw(ATA_PRIMARY_DATA_PORT);
        // TODO: Store the data in a struct
    }

    return 1;
}

void select_device(uint8_t device) {
    if (device != last_selected_device) {
        outb(ATA_PRIMARY_DRIVE_PORT, device);
        last_selected_device = device;

        // Read the status register 15 times after device selection (waits 400ns)
        for (int i = 0; i < 15; i++) {
            inb(ATA_PRIMARY_ALT_STATUS_PORT);
        }
    }
}

int detect_device(void) {
    select_device(ATA_MASTER_DRIVE);
    wait_not_busy();

    uint8_t status = read_status();
    uint8_t lba_mid = inb(ATA_PRIMARY_LBA_MID_PORT);
    uint8_t lba_high = inb(ATA_PRIMARY_LBA_HIGH_PORT);
    if (status == ATA_FLOATING_BUS && lba_mid == ATA_FLOATING_BUS && lba_high == ATA_FLOATING_BUS) {    // No drive
        return 0;
    }

    return 1;    // Drive detected
}

void setup_drive_read_write(uint8_t device, uint32_t lba, uint8_t sector_count) {
    select_device(ATA_MASTER_DRIVE);
    wait_not_busy();
    wait_ready();

    uint8_t drive_byte = (device == ATA_SLAVE_DRIVE) ? 0xF0 : 0xE0; // default to master drive (0xE0)

    // from "28 bit PIO" section of "https://wiki.osdev.org/ATA_PIO_Mode"
    outb(ATA_PRIMARY_DRIVE_PORT, drive_byte | ((lba >> 24) & 0x0F));
    outb(ATA_PRIMARY_SECTOR_COUNT_PORT, sector_count);
    outb(ATA_PRIMARY_LBA_LOW_PORT, (uint8_t)(lba));
    outb(ATA_PRIMARY_LBA_MID_PORT, (uint8_t)(lba >> 8));
    outb(ATA_PRIMARY_LBA_HIGH_PORT, (uint8_t)(lba >> 16));
}

int read_sectors(uint32_t lba, uint8_t sector_count, uint8_t* buffer) {
    setup_drive_read_write(ATA_MASTER_DRIVE, lba, sector_count);
    outb(ATA_PRIMARY_COMMAND_PORT, ATA_READ_SECTORS_CMD);

    // read data

    return 1;
}

int write_sectors(uint32_t lba, uint8_t sector_count, const uint8_t* buffer) {
    setup_drive_read_write(ATA_MASTER_DRIVE, lba, sector_count);
    outb(ATA_PRIMARY_COMMAND_PORT, ATA_WRITE_SECTORS_CMD);

    // write data
    const uint16_t* buf = (const uint16_t*)buffer;
    for (int sector = 0; sector < sector_count; sector++) {
        wait_not_busy();
        wait_ready();

        // Write 512 bytes for each sector
        for (int i = 0; i < 256; i++) {
            outw(ATA_PRIMARY_DATA_PORT, buf[i]);
        }
        buf += 256;

        outb(ATA_PRIMARY_COMMAND_PORT, ATA_FLUSH_CACHE_CMD);
        wait_not_busy();
    }

    return 1;
}

void init_disk(void) {
    println("[DISK] Initializing disk...");

    if (!detect_device()) {
        println("[DISK] No drive detected");
        return;
    }

	print("[DISK] Disk status: ");
	println(read_status_str());

    if (identify_device()) {
        println("[DISK] ATA disk initialized");
    } else {
        println("[DISK] Failed to initialize ATA disk");
    }
}