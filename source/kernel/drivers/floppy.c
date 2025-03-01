// using source: https://wiki.osdev.org/Floppy_Disk_Controller

#include "floppy.h"
#include "../core/x86.h"
#include "../core/stdio.h"
#include "../core/idt.h"
#include <stdbool.h>

#define SECTOR_SIZE 512

extern void irq6(void);

FloppyController Controller = {
    .DOR  = 0x3F2,
    .MSR  = 0x3F4,
    .FIFO = 0x3F5,
    .CCR  = 0x3F7
};

volatile bool ReceivedIRQ = false;

void lba_2_chs(uint32_t lba, uint16_t* cyl, uint16_t* head, uint16_t* sector)
{
    *cyl    = lba / (2 * FLOPPY_144_SECTORS_PER_TRACK);
    *head   = ((lba % (2 * FLOPPY_144_SECTORS_PER_TRACK)) / FLOPPY_144_SECTORS_PER_TRACK);
    *sector = ((lba % (2 * FLOPPY_144_SECTORS_PER_TRACK)) % FLOPPY_144_SECTORS_PER_TRACK + 1);
}

uint8_t read_data_byte(void) {
    // Wait until the controller is ready to send data.
    while (!(x86_inb(Controller.MSR) & 0x80));
    return x86_inb(Controller.FIFO);
}


void floppy_handler()
{
    ReceivedIRQ = true;
}

void send_command(uint8_t command) {
    // Wait until the controller is ready.
    while (!(x86_inb(Controller.MSR) & 0x80));

    x86_outb(Controller.FIFO, command);
}

void reset_floppy() {
    ReceivedIRQ = false; 	// This will prevent the FDC from being faster than us!

    // Enter, then exit reset mode.
    x86_outb(Controller.DOR,0x00);
    x86_outb(Controller.DOR,0x0C);

    while(!ReceivedIRQ) ;	// Wait for the IRQ handler to run

    // sense interrupt -- 4 of them typically required after a reset
    for (int i = 4 ; i > 0 ; --i)
    {
        send_command(SENSE_INTERRUPT);
        read_data_byte();
        read_data_byte();
    }

    x86_outb(Controller.CCR,0x00);	// 500Kbps -- for 1.44M floppy

    // configure the drive
    send_command(SPECIFY);
    x86_outb(Controller.FIFO, STEPRATE_HEADUNLOAD);
    x86_outb(Controller.FIFO, HEADLOAD_NDMA);
}

void init_floppy(void) {
    printf("[I/O] Initializing Floppy..\r\n");

    register_interrupt_handler(38, (uint32_t)irq6);

    enable_irq(6);

    printf("[I/O] Floppy initialized\r\n");
}

bool FLOPPY_ReadSectors(uint32_t lba, uint32_t sector_count, void* buffer) {
    uint8_t* buf = (uint8_t*)buffer;

    for (uint32_t i = 0; i < sector_count; i++) {
        // Convert LBA to CHS.
        uint16_t cyl, head, sector;
        lba_2_chs(lba + i, &cyl, &head, &sector);

        // Clear the IRQ flag.
        ReceivedIRQ = false;

        // Begin the read command sequence.
        // According to OSDev, the READ_DATA command (code 6) expects:
        // Byte 0: READ_DATA (6)
        // Byte 1: (drive << 2) | head (using drive 0 here)
        // Byte 2: Cylinder
        // Byte 3: Head
        // Byte 4: Sector (starting at 1)
        // Byte 5: Bytes per sector code (2 for 512 bytes, since 128 << 2 == 512)
        // Byte 6: Last sector number (for a 1.44MB floppy, typically 18)
        // Byte 7: Gap length (usually 27)
        // Byte 8: Data length (0xFF, unused)
        send_command(READ_DATA);
        x86_outb(Controller.FIFO, (0 << 2) | head);  // drive 0, head info
        x86_outb(Controller.FIFO, cyl);
        x86_outb(Controller.FIFO, head);
        x86_outb(Controller.FIFO, sector);
        x86_outb(Controller.FIFO, 2);  // 2 means 512 bytes per sector (128*2^2)
        x86_outb(Controller.FIFO, FLOPPY_144_SECTORS_PER_TRACK);  // assuming 18 sectors per track
        x86_outb(Controller.FIFO, 27);  // gap length
        x86_outb(Controller.FIFO, 0xFF);  // filler

        // Wait for the IRQ to signal that data is ready.
        while (!ReceivedIRQ) {
            // Optionally, insert a timeout or sleep routine.
        }

        // Read one sector (512 bytes) from the FIFO.
        for (int j = 0; j < SECTOR_SIZE; j++) {
            buf[i * SECTOR_SIZE + j] = read_data_byte();
        }
    }

    return true;
}