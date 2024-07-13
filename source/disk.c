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

#define ATA_IDENTIFY_DRIVE_CMD 0xEC

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
    } while ((status & ATA_STATUS_BSY) && !(status & (ATA_STATUS_ERR | ATA_STATUS_DRQ)));

    if (status & ATA_STATUS_ERR) {
        println("[DISK] Error during device identification");
        return 0;
    }

    for (int i = 0; i < 256; i++) {
        uint16_t data = inb(ATA_PRIMARY_DATA_PORT) | (inb(ATA_PRIMARY_DATA_PORT) << 8);
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
    if (status == ATA_FLOATING_BUS) {    // No drive
        return 0;
    }
    return 1;    // Drive detected
}

void init_disk(void) {
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