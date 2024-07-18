//
// Created by Alan on 12/07/2024.
// Based on: https://wiki.osdev.org/FAT
//

#include "fat.h"
#include "disk.h"
#include "display.h"

static FAT_Filesystem current_filesystem;

void init_fat(void) {
    uint8_t boot_sector[SECTOR_SIZE];
    if (!read_sectors(0, 1, boot_sector)) {    // read boot sector into buffer
        println("[FAT] Unable to read boot sector");
        return;
    }

    // parse BIOS parameter block
    FAT_BPB_Bits* BPB = (FAT_BPB_Bits*)boot_sector;

    uint32_t total_sectors = (BPB->TotalSectors == 0) ? BPB->SectorCountLarge : BPB->TotalSectors;
    uint32_t fat_size = (BPB->SectorsPerFat == 0) ? ((FAT32_EBPB_Bits*)(boot_sector + 36))->SectorsPerFat : BPB->SectorsPerFat;
    uint32_t root_dir_sectors = ((BPB->RootEntries * 32) + (BPB->BytesPerSector - 1)) / BPB->BytesPerSector;
    uint32_t data_sectors = total_sectors - (BPB->ReservedSectors + (BPB->FATCount * fat_size) + root_dir_sectors);
    uint32_t total_clusters = data_sectors / BPB->SectorsPerCluster;

    current_filesystem.type = get_fat_type(total_clusters, BPB->BytesPerSector);

    // store filesystem information depending on type
    switch (current_filesystem.type) {
        case FAT12:
            println("[FAT] FAT12 ...");
            break;
        case FAT16:
            println("[FAT] FAT16 ...");
            break;
        case FAT32:
            println("[FAT] FAT32 ...");
            break;
        default:
            println("[FAT] Unknown/Unsupported FAT type");
            return;
    }

    println("[FAT] FAT filesystem initialized");
}

FATType get_fat_type(uint32_t total_clusters, uint16_t bytes_per_sector) {
    // ExFAT currently not supported
    if (total_clusters < 4085) {
        return FAT12;
    } else if (total_clusters < 65525) {
        return FAT16;
    } else {
        return FAT32;
    }
}