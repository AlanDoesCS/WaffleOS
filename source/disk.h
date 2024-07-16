//
// Created by Alan on 12/07/2024.
//

#ifndef DISK_H
#define DISK_H

#include "types.h"

#define SECTOR_SIZE 512

// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ata/ns-ata-_identify_device_data?redirectedfrom=MSDN
typedef struct {
    uint16_t GeneralConfiguration;
    uint16_t NumCylinders;
    uint16_t NumHeads;
    uint16_t NumSectorsPerTrack;
    uint8_t SerialNumber[20];
    uint8_t FirmwareRevision[8];
    uint8_t ModelNumber[40];
    uint16_t Capabilities[2];
    uint32_t UserAddressableSectors;
    uint32_t total_sectors_lba48[2];     // 64 bit value
    uint16_t MultiWordDMASupport;        // in case DMA support is added later on
    uint16_t MultiWordDMAActive;
    uint16_t AdvancedPIOModes;
} ATA_IDENTIFY_DEVICE_DATA;

uint8_t read_status(void);
void select_device(uint8_t device);
void init_disk(void);
void wait_not_busy(void);
void wait_ready(void);
int read_sectors(uint32_t lba, uint8_t sector_count, uint8_t* buffer);
int write_sectors(uint32_t lba, uint8_t sector_count, const uint8_t* buffer);
int identify_device(ATA_IDENTIFY_DEVICE_DATA* device_info);

#endif //DISK_H
