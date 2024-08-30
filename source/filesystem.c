//
// Created by Alan on 12/07/2024.
// Based on: https://wiki.osdev.org/FAT
// and: https://wiki.osdev.org/GPT
// and: https://uefi.org/specs/UEFI/2.10/05_GUID_Partition_Table_Format.html

#include "filesystem.h"
#include "disk.h"
#include "display.h"
#include "str.h"
#include "types.h"
#include "memory.h"

#include "timer.h"

#define GPT_PROTECTIVE_MBR 0xEE
#define UEFI_SYSTEM_PARTITION 0xEF

static FAT_Filesystem current_filesystem;

void print_guid(uint8_t* guid) {
    for (int i = 0; i < 16; i++) {
        print_uint8(guid[i]);
    }
}

void parse_guid_partition_table(uint8_t* gpt_sector) {
}

void init_filesystem(void) {
    uint8_t boot_sector[SECTOR_SIZE];
    if (!read_sectors(0, 1, boot_sector)) {    // read boot sector into buffer
        println("[FS] Unable to read boot sector");
        return;
    }

    if (boot_sector[510] != 0x55 || boot_sector[511] != 0xAA) {
        println("[FS] Invalid boot sector signature");
        return;
    }

    FAT_BPB_Bits* BPB = (FAT_BPB_Bits*)boot_sector;

    // Print sectors per cluster for debugging
    print("[FS] Sectors per cluster: ");
    print_uint8(BPB->SectorsPerCluster);
    print_char('\n');

    init_fat_partition(0, boot_sector);
}

void parse_gpt(void) {
    return;
}

void parse_mbr(LegacyMBR* mbr) {
    return;
}

void init_fat_partition(uint32_t partition_start_lba, uint8_t* boot_sector) {
    // parse BIOS parameter block
    FAT_BPB_Bits* BPB = (FAT_BPB_Bits*)boot_sector;

    uint32_t total_sectors = (BPB->TotalSectors == 0) ? BPB->SectorCountLarge : BPB->TotalSectors;
	uint32_t fat_size = (BPB->SectorsPerFat == 0) ? ((FAT32_EBPB_Bits*)(boot_sector + 36))->SectorsPerFat : BPB->SectorsPerFat;
	uint32_t root_dir_sectors = ((BPB->RootEntries * 32) + (BPB->BytesPerSector - 1)) / BPB->BytesPerSector;
	uint32_t data_sectors = total_sectors - (BPB->ReservedSectors + (BPB->FATCount * fat_size) + root_dir_sectors);

	if (BPB->SectorsPerCluster == 0) {
        println("[FAT] Error: SectorsPerCluster is 0");
        return;
    }

    uint32_t total_clusters = data_sectors / BPB->SectorsPerCluster;

    current_filesystem.type = get_fat_type(total_clusters, BPB->BytesPerSector);

    // store filesystem information depending on type
    print("[FAT] Filesystem type: ");
    switch (current_filesystem.type) {
        case FAT12:
            println("FAT12");
            current_filesystem.FAT12_FS.BPB = *BPB;
            memcpy(&current_filesystem.FAT12_FS.EBPB, boot_sector + 36, sizeof(FAT12_EBPB_Bits));
            current_filesystem.FAT12_FS.FirstFATSector = BPB->ReservedSectors;
            current_filesystem.FAT12_FS.RootDirSector = BPB->ReservedSectors + (BPB->FATCount * BPB->SectorsPerFat);
            current_filesystem.FAT12_FS.FirstDataSector = current_filesystem.FAT12_FS.RootDirSector + root_dir_sectors;
            current_filesystem.FAT12_FS.RootDirSectors = root_dir_sectors;
            break;
        case FAT16:
            println("FAT16");
            current_filesystem.FAT16_FS.BPB = *BPB;
            memcpy(&current_filesystem.FAT16_FS.EBPB, boot_sector + 36, sizeof(FAT16_EBPB_Bits));
            current_filesystem.FAT16_FS.FirstFATSector = BPB->ReservedSectors;
            current_filesystem.FAT16_FS.RootDirSector = BPB->ReservedSectors + (BPB->FATCount * BPB->SectorsPerFat);
            current_filesystem.FAT16_FS.FirstDataSector = current_filesystem.FAT16_FS.RootDirSector + root_dir_sectors;
            current_filesystem.FAT16_FS.RootDirSectors = root_dir_sectors;
            break;
        case FAT32:
            println("FAT32");
            current_filesystem.FAT32_FS.BPB = *BPB;
            memcpy(&current_filesystem.FAT32_FS.EBPB, boot_sector + 36, sizeof(FAT32_EBPB_Bits));
            current_filesystem.FAT32_FS.FirstFATSector = BPB->ReservedSectors;
            current_filesystem.FAT32_FS.RootDirSector = ((FAT32_EBPB_Bits*)(boot_sector + 36))->RootCluster;
            current_filesystem.FAT32_FS.FirstDataSector = BPB->ReservedSectors + (BPB->FATCount * fat_size);
            current_filesystem.FAT32_FS.RootDirSectors = 0; // FAT32 has no fixed root dir
            break;
        default:
            println("Unknown/Unsupported FAT type");
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