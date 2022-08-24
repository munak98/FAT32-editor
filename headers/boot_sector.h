/*
This header file defines the boot sector
structure acording to the attributes and offsets given in the
Microsoft Extensible Firmware Initiative FAT32 File System Specification,
Version 1.03
 */

#ifndef BOOT_SEC_H
#define BOOT_SEC_H

#include <inttypes.h>

#pragma pack(push)
#pragma pack(1)

struct boot_sector_struct
{
    uint8_t jmpBoot[3];     // Jump Code + NOP
    char OEMName[8];        // OEM Name (Probably MSWIN4.1)
    uint16_t BytesPerSec;   // Bytes Per Sector
    uint8_t SecPerClus;     // Sectors Per Cluster
    uint16_t RsvdSecCnt;    // Reserved Sectors
    uint8_t NumFATs;        // Number of Copies of FAT
    uint16_t RootEntCnt;    // Maximum Root Directory Entries (N/A for FAT32)
    uint16_t TotSec16;      // Number of Sectors in Partition Smaller than 32MB (N/A for FAT32)
    uint8_t Media;          // Media Descriptor (F8h forHard Disks)
    uint16_t FATSz16;       // Sectors Per FAT in Older FATSystems (N/A for FAT32)
    uint16_t SecPerTrk;     // Sectors Per Track
    uint16_t NumHeads;      // Number of Heads
    uint32_t HiddSec;       // Number of Hidden Sectors in Partition
    uint32_t TotSec32;      // Number of Sectors in Partition
    uint32_t FATSz32;       // Number of Sectors Per FAT
    uint16_t ExtFlags;      // Flags
    uint8_t FSVerLow;       // Version of FAT32 Drive Low Byte
    uint8_t FSVerHigh;      // Version of FAT32 Drive High Byte
    uint32_t RootClus;      // Cluster Number of the Start of the Root Directory
    uint16_t FSInfo;        // Sector Number of the FileSystem Information Sector
    uint16_t BkBootSec;     // Sector Number of the BackupBoot Sector
    char reserved[12];      // Reserved
    uint8_t DrvNum;         // Logical Drive Number of Partition
    uint8_t Reserved1;      // Unused
    uint8_t BootSig;        // Extended Signature
    uint32_t VolID;         // Serial Number of Partition
    char VolLab[11];        // Volume Name of Partition
    char FilSysType[8];     // FAT Name (FAT32)
    char CodeReserved[420]; // Executable Code
    uint8_t SigA;           // Boot Record Signature A
    uint8_t SigB;           // Boot Record Signature B
};

#pragma pack(pop)

typedef struct boot_sector_struct boot_sector;

void show_device_info(boot_sector *);
void check_sig(boot_sector *);

void read_boot_sector(int, boot_sector *);

extern boot_sector *bootSector;

#endif
