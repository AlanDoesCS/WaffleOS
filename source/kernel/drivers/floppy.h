// using source: https://wiki.osdev.org/Floppy_Disk_Controller
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define FLOPPY_144_SECTORS_PER_TRACK 18

#define STEPRATE_HEADUNLOAD 0xDF
#define HEADLOAD_NDMA       0x02

enum FloppyRegisters
{
    STATUS_REGISTER_A                = 0x3F0, // read-only
    STATUS_REGISTER_B                = 0x3F1, // read-only
    DIGITAL_OUTPUT_REGISTER          = 0x3F2,
    TAPE_DRIVE_REGISTER              = 0x3F3,
    MAIN_STATUS_REGISTER             = 0x3F4, // read-only
    DATARATE_SELECT_REGISTER         = 0x3F4, // write-only
    DATA_FIFO                        = 0x3F5,
    DIGITAL_INPUT_REGISTER           = 0x3F7, // read-only
    CONFIGURATION_CONTROL_REGISTER   = 0x3F7  // write-only
};

enum FloppyCommands
{
    READ_TRACK =                 2,	     // generates IRQ6
    SPECIFY =                    3,      // * set drive parameters
    SENSE_DRIVE_STATUS =         4,
    WRITE_DATA =                 5,      // * write to the disk
    READ_DATA =                  6,      // * read from the disk
    RECALIBRATE =                7,      // * seek to cylinder 0
    SENSE_INTERRUPT =            8,      // * ack IRQ6, get status of last command
    WRITE_DELETED_DATA =         9,
    READ_ID =                    10,	 // generates IRQ6
    READ_DELETED_DATA =          12,
    FORMAT_TRACK =               13,     // *
    DUMPREG =                    14,
    SEEK =                       15,     // * seek both heads to cylinder X
    VERSION =                    16,	// * used during initialization, once
    SCAN_EQUAL =                 17,
    PERPENDICULAR_MODE =         18,	// * used during initialization, once, maybe
    CONFIGURE =                  19,     // * set controller parameters
    LOCK =                       20,     // * protect controller params from a reset
    VERIFY =                     22,
    SCAN_LOW_OR_EQUAL =          25,
    SCAN_HIGH_OR_EQUAL =         29
};

typedef struct {
    uint16_t DOR;   // Digital Output Register (0x3F2)
    uint16_t MSR;   // Main Status Register (0x3F4)
    uint16_t FIFO;  // Data Register (0x3F5)
    uint16_t CCR;   // Configuration Control Register (0x3F7)
} FloppyController;

// Declare a global instance with the correct hex codes.
extern FloppyController Controller;

void send_command(uint8_t command);
void reset_floppy();
void init_floppy(void);
void lba_2_chs(uint32_t lba, uint16_t* cyl, uint16_t* head, uint16_t* sector);
uint8_t read_data_byte(void);
bool FLOPPY_ReadSectors(uint32_t lba, uint32_t sector_count, void* buffer);
