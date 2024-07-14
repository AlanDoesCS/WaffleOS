//
// Created by Alan on 12/07/2024.
//

#ifndef DISK_H
#define DISK_H

#include "types.h"

#define SECTOR_SIZE 512

uint8_t read_status(void);
void select_device(uint8_t device);
void init_disk(void);
void wait_not_busy(void);
void wait_ready(void);
int read_sectors(uint32_t lba, uint8_t sector_count, uint8_t* buffer);
int write_sectors(uint32_t lba, uint8_t sector_count, const uint8_t* buffer);

#endif //DISK_H
