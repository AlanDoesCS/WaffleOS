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

    // check for GPT or MBR
    LegacyMBR* mbr = (LegacyMBR*)boot_sector;
    if (mbr->PartitionRecord[0].OSType == GPT_PROTECTIVE_MBR) {
        println("[FS] GPT Protective MBR detected");
        parse_gpt();
        return;
    } else {
        println("[FS] MBR detected");
        parse_mbr(mbr);
    }

    // TODO: FAT
}

void parse_gpt(void) {
    return;
}

void parse_mbr(LegacyMBR* mbr) {
    return;
}

void init_fat_partition(uint32_t partition_start_lba) {
    uint8_t boot_sector[SECTOR_SIZE];
    if (!read_sectors(partition_start_lba, 1, boot_sector)) {    // read boot sector into buffer
        println("[FAT] Unable to read boot sector");
        return;
    }

	if (boot_sector[510] != 0x55 || boot_sector[511] != 0xAA) {
        println("[FAT] Invalid boot sector signature");
        return;
    }
	sleep(2);

    // parse BIOS parameter block
    FAT_BPB_Bits* BPB = (FAT_BPB_Bits*)boot_sector;

    char oem_name[9];
    memcpy(oem_name, BPB->OEM_ID, 8);
    oem_name[8] = '\0';
    print("[FAT] OEM Name: \"");
    print(oem_name);
	println("\"");

    // Print key BPB fields
    print("[FAT] Bytes per sector: ");
    print_uint16(BPB->BytesPerSector);
    println("");

    print("[FAT] Sectors per cluster: ");
    print_uint8(BPB->SectorsPerCluster);
    println("");

    uint32_t total_sectors = (BPB->TotalSectors == 0) ? BPB->SectorCountLarge : BPB->TotalSectors;
	uint32_t fat_size = (BPB->SectorsPerFat == 0) ? ((FAT32_EBPB_Bits*)(boot_sector + 36))->SectorsPerFat : BPB->SectorsPerFat;
	uint32_t root_dir_sectors = ((BPB->RootEntries * 32) + (BPB->BytesPerSector - 1)) / BPB->BytesPerSector;
	uint32_t data_sectors = total_sectors - (BPB->ReservedSectors + (BPB->FATCount * fat_size) + root_dir_sectors);

	print("[FAT] Total sectors: ");
    print_uint32(total_sectors);
    print("\n");

    print("[FAT] FAT size: ");
    print_uint32(fat_size);
    print("\n");

    print("[FAT] Root directory sectors: ");
    print_uint32(root_dir_sectors);
    print("\n");

    print("[FAT] Data sectors: ");
    print_uint32(data_sectors);
    print("\n");

    print("[FAT] Sectors per cluster: ");
    print_uint32(BPB->SectorsPerCluster);
    print("\n");

	if (BPB->SectorsPerCluster == 0) {
        println("[FAT] Error: SectorsPerCluster is 0");
        return;
    }

	uint32_t total_clusters = data_sectors / BPB->SectorsPerCluster;
	println("[FAT] 6");
	sleep(2);

    current_filesystem.type = get_fat_type(total_clusters, BPB->BytesPerSector);

    // store filesystem information depending on type
    switch (current_filesystem.type) {
        case FAT12:
            current_filesystem.FAT12_FS.BPB = *BPB;
            memcpy(&current_filesystem.FAT12_FS.EBPB, boot_sector + 36, sizeof(FAT12_EBPB_Bits));
            current_filesystem.FAT12_FS.FirstFATSector = BPB->ReservedSectors;
            current_filesystem.FAT12_FS.RootDirSector = BPB->ReservedSectors + (BPB->FATCount * BPB->SectorsPerFat);
            current_filesystem.FAT12_FS.FirstDataSector = current_filesystem.FAT12_FS.RootDirSector + root_dir_sectors;
            current_filesystem.FAT12_FS.RootDirSectors = root_dir_sectors;
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