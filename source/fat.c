//
// Created by Alan on 12/07/2024.
// Based on: https://wiki.osdev.org/FAT#File_Allocation_Table
//

#include "fat.h"

FATType get_fat_type(uint32_t total_clusters, uint16_t sectorsize) {
    if (sectorsize == 0) {
        return ExFAT;
    } else if (total_clusters < 4085) {
        return FAT12;
    } else if (total_clusters < 65525) {
        return FAT16;
    } else {
        return FAT32;
    }
}