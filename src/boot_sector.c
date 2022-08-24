#include "../headers/boot_sector.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void show_device_info(boot_sector *sector)
{
    printf("Boot sector and BIOS Parameter Block information\n");
    printf("jmpBoot: ");
    for (int i = 0; i < 3; i++) // Jump Code + NOP
        printf("0x%X ", sector->jmpBoot[i]);
    printf("\n");
    printf("OEM_Identifier: ");
    for (int i = 0; i < 8; i++) // OEM Name (Probably MSWIN4.1)
        printf("%c", sector->OEMName[i]);
    printf("\n");
    printf("BytesPerSec: %" PRIu16 "\n", sector->BytesPerSec); // Bytes Per Sector
    printf("SecPerClus: %" PRIu8 "\n", sector->SecPerClus);    // Sectors Per Cluster
    printf("RsvdSecCnt: %" PRIu16 "\n", sector->RsvdSecCnt);   // Reserved Sectors
    printf("NumFATs: %" PRIu8 "\n", sector->NumFATs);          // Number of Copies of FAT
    printf("NumRootEnt: %" PRIu16 "\n", sector->RootEntCnt);   // Maximum Root DirectoryEntries (N/A for FAT32)
    printf("TotSec16: %" PRIu16 "\n", sector->TotSec16);       // Number of Sectors inPartition Smaller than 32MB (N/A for FAT32)
    printf("Media: 0x%X\n", sector->Media);                    // Media Descriptor (F8h forHard Disks)
    printf("SecPerFAT: %" PRIu16 "\n", sector->FATSz16);       // Sectors Per FAT in Older FATSystems (N/A for FAT32)
    printf("SecPerTrk: %" PRIu16 "\n", sector->SecPerTrk);     // Sectors Per Track
    printf("NumHeads: %" PRIu16 "\n", sector->NumHeads);       // Number of Heads
    printf("HiddSec: %" PRIu32 "\n", sector->HiddSec);         // Number of Hidden Sectors inPartition
    printf("TotSec32: %" PRIu32 "\n", sector->TotSec32);       // Number of Sectors inPartition
    printf("FATSz32: %" PRIu32 "\n", sector->FATSz32);         // Number of Sectors Per FAT
    printf("ExtFlags: %" PRIu16 "\n", sector->ExtFlags);       // Flags
    printf("FSVerLow: %" PRIu8 "\n", sector->FSVerLow);        // Version of FAT32 Drive Low Byte
    printf("FSVerHigh: %" PRIu8 "\n", sector->FSVerHigh);      // Version of FAT32 Drive High Byte
    printf("RootClus: %" PRIu32 "\n", sector->RootClus);       // Cluster Number of the Start of the Root Directory
    printf("FSInfo: %" PRIu16 "\n", sector->FSInfo);           // Sector Number of the FileSystem Information Sector
    printf("BkBootSec: %" PRIu16 "\n", sector->BkBootSec);     // Sector Number of the BackupBoot Sector
    printf("Reserved: ");
    for (int i = 0; i < 12; i++) // Reserved
        printf("%" PRIu8, sector->reserved[i]);
    printf("\n");
    printf("DrvNum: 0x%X\n", sector->DrvNum);             // Logical Drive Number ofPartition
    printf("Reserved1: %" PRIu8 "\n", sector->Reserved1); // Unused
    printf("BootSig: 0x%X\n", sector->BootSig);           // Extended Signature
    printf("VolID: %" PRIu32 "\n", sector->VolID);        // Serial Number of Partition
    printf("Vol Partition: ");
    for (int i = 0; i < 11; i++) // Volume Name of Partition
        printf("%c", sector->VolLab[i]);
    printf("\n");
    printf("FilSysType: ");
    for (int i = 0; i < 8; i++) // FAT Name (FAT32)
        printf("%c", sector->FilSysType[i]);
    printf("\n");
    printf("SigA: 0x%X\n", sector->SigA); // Boot Record Signature A
    printf("SigB: 0x%X\n", sector->SigB); // Boot Record Signature B
}

void check_sig(boot_sector *sector)
{
    if ((sector->SigA != 0x55) || (sector->SigB != 0xAA))
    {
        printf("Invalid boot sector signature!\n");
        exit(-1);
    }
}

void read_boot_sector(int fd, boot_sector *bootSector)
{
    if (read(fd, bootSector, 512) == -1)
        perror("read");

    show_device_info(bootSector);

    check_sig(bootSector);
};
