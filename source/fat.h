//
// Created by Alan on 12/07/2024.
// Based on: https://wiki.osdev.org/FAT
//

#ifndef FAT_H
#define FAT_H

#include "types.h"
#include "disk.h"

#define MAX_PATH_LENGTH 256

typedef enum {
    FAT12,
    FAT16,
    FAT32,
    ExFAT,
    Unknown
} FATType;

typedef struct {
    uint8_t  Name[8];
    uint8_t  Extension[3];
    uint8_t  Attributes;            // READ_ONLY=0x01 HIDDEN=0x02 SYSTEM=0x04 VOLUME_ID=0x08 DIRECTORY=0x10 ARCHIVE=0x20 LFN=READ_ONLY|HIDDEN|SYSTEM|VOLUME_ID
    uint8_t  Reserved;
    uint8_t  CreationTimeHundredths;
    uint16_t CreationTime;          // 5/6/5 bits for hour/minute/second - Multiply seconds by 2
    uint16_t CreationDate;          // 7/4/5 bits for year/month/day
    uint16_t LastAccessDate;        // 7/4/5 bits for year/month/day
    uint16_t FirstClusterHigh;      // FAT32 only (0 for FAT12/16)
    uint16_t LastWriteTime;         // 5/6/5 bits for hour/minute/second - Multiply seconds by 2
    uint16_t LastWriteDate;         // 7/4/5 bits for year/month/day
    uint16_t FirstClusterLow;
    uint32_t FileSize;
} __attribute__((packed)) FAT_DirectoryEntry;

typedef struct {
    uint8_t  SequenceNumber;
    uint16_t Name1[5];
    uint8_t  Attributes;
    uint8_t  Type;
    uint8_t  Checksum;
    uint16_t Name2[6];
    uint16_t ZERO;
    uint16_t Name3[2];
} __attribute__((packed)) FAT_ShortFilenameEntry;

typedef struct {
    uint8_t  Jump[3];
    uint8_t  OEM_ID[8];
    uint16_t BytesPerSector;
    uint8_t  SectorsPerCluster;
    uint16_t ReservedSectors;
    uint8_t  FATCount;
    uint16_t RootEntries;
    uint16_t TotalSectors;
    uint8_t  MediaDescriptorType;
    uint16_t SectorsPerFat;
    uint16_t SectorsPerTrack;
    uint16_t Heads;
    uint32_t HiddenSectors;
    uint32_t SectorCountLarge;    // set if more than 65535 sectors
} __attribute__((packed)) FAT_BPB_Bits;

typedef struct {
    uint8_t  DriveNumber;
    uint8_t  Reserved;
    uint8_t  Signature;           // MUST BE 0x28 or 0x29
    uint32_t VolumeID;            // Serial number of the volume
    uint8_t  VolumeLabel[11];
    uint8_t  SystemID[8];         // File system type (Don't trust)
    uint8_t  BootCode[448];
    uint16_t BootSignature;       // MUST BE 0xAA55
} __attribute__((packed)) FAT12_EBPB_Bits;

typedef struct {
    uint8_t  DriveNumber;
    uint8_t  Reserved;
    uint8_t  Signature;           // MUST BE 0x28 or 0x29
    uint32_t VolumeID;            // Serial number of the volume
    uint8_t  VolumeLabel[11];
    uint8_t  SystemID[8];         // File system type (Don't trust)
    uint8_t  BootCode[448];
    uint16_t BootSignature;       // MUST BE 0xAA55
} __attribute__((packed)) FAT16_EBPB_Bits;

typedef struct {
    uint32_t SectorsPerFat;
    uint16_t Flags;
    uint16_t Version;
    uint32_t RootCluster;
    uint16_t FSInfo;
    uint16_t BackupBootSector;
    uint8_t  Reserved[12];
    uint8_t  DriveNumber;
    uint8_t  Reserved1;
    uint8_t  Signature;           // MUST BE 0x28 or 0x29
    uint32_t VolumeID;            // Serial number of the volume
    uint8_t  VolumeLabel[11];
    uint8_t  SystemID[8];         // File system type (Don't trust)
    uint8_t  BootCode[420];
    uint16_t BootSignature;       // MUST BE 0xAA55
} __attribute__((packed)) FAT32_EBPB_Bits;

typedef struct {
    FAT_BPB_Bits BPB;
    FAT12_EBPB_Bits EBPB;
    uint32_t FirstFATSector;
    uint32_t FirstDataSector;
    uint32_t RootDirSector;
    uint32_t RootDirSectors;
} __attribute__((packed)) FAT12_Filesystem;

typedef struct {
    FAT_BPB_Bits BPB;
    FAT16_EBPB_Bits EBPB;
    uint32_t FirstFATSector;
    uint32_t FirstDataSector;
    uint32_t RootDirSector;
    uint32_t RootDirSectors;
} __attribute__((packed)) FAT16_Filesystem;

typedef struct {
    FAT_BPB_Bits BPB;
    FAT32_EBPB_Bits EBPB;
    uint32_t FirstFATSector;
    uint32_t FirstDataSector;
    uint32_t RootDirSector;
    uint32_t RootDirSectors;
} __attribute__((packed)) FAT32_Filesystem;

typedef struct {
    FATType type;
    union {
        FAT12_Filesystem FAT12_FS;
        FAT16_Filesystem FAT16_FS;
        FAT32_Filesystem FAT32_FS;
    };
} __attribute__((packed)) FAT_Filesystem;

void init_fat(void);
FATType get_fat_type(uint32_t total_clusters, uint16_t bytes_per_sector);



#endif //FAT_H
