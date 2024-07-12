//
// Created by Alan on 12/07/2024.
// Based on: https://wiki.osdev.org/FAT#File_Allocation_Table
//

#ifndef FAT_H
#define FAT_H

#include "types.h"

typedef enum {
    FAT12,
    FAT16,
    FAT32,
    ExFAT,
    Unknown
} FATType;

void init_fat(void);


#endif //FAT_H
