#include "../headers/boot_sector.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void show_device_info()
{
    printf("Boot sector and BIOS Parameter Block information\n");
    printf("jmpBoot: ");
    for (int i = 0; i < 3; i++) // Jump Code + NOP
        printf("0x%X ", BS->jmpBoot[i]);
    printf("\n");
    printf("OEM_Identifier: ");
    for (int i = 0; i < 8; i++) // OEM Name (Probably MSWIN4.1)
        printf("%c", BS->OEMName[i]);
    printf("\n");
    printf("BytesPerSec: %" PRIu16 "\n", BS->BytesPerSec); // Bytes Per Sector
    printf("SecPerClus: %" PRIu8 "\n", BS->SecPerClus);    // Sectors Per Cluster
    printf("RsvdSecCnt: %" PRIu16 "\n", BS->RsvdSecCnt);   // Reserved Sectors
    printf("NumFATs: %" PRIu8 "\n", BS->NumFATs);          // Number of Copies of FAT
    printf("NumRootEnt: %" PRIu16 "\n", BS->RootEntCnt);   // Maximum Root DirectoryEntries (N/A for FAT32)
    printf("TotSec16: %" PRIu16 "\n", BS->TotSec16);       // Number of Sectors inPartition Smaller than 32MB (N/A for FAT32)
    printf("Media: 0x%X\n", BS->Media);                    // Media Descriptor (F8h forHard Disks)
    printf("SecPerFAT: %" PRIu16 "\n", BS->FATSz16);       // Sectors Per FAT in Older FATSystems (N/A for FAT32)
    printf("SecPerTrk: %" PRIu16 "\n", BS->SecPerTrk);     // Sectors Per Track
    printf("NumHeads: %" PRIu16 "\n", BS->NumHeads);       // Number of Heads
    printf("HiddSec: %" PRIu32 "\n", BS->HiddSec);         // Number of Hidden Sectors inPartition
    printf("TotSec32: %" PRIu32 "\n", BS->TotSec32);       // Number of Sectors inPartition
    printf("FATSz32: %" PRIu32 "\n", BS->FATSz32);         // Number of Sectors Per FAT
    printf("ExtFlags: %" PRIu16 "\n", BS->ExtFlags);       // Flags
    printf("FSVerLow: %" PRIu8 "\n", BS->FSVerLow);        // Version of FAT32 Drive Low Byte
    printf("FSVerHigh: %" PRIu8 "\n", BS->FSVerHigh);      // Version of FAT32 Drive High Byte
    printf("RootClus: %" PRIu32 "\n", BS->RootClus);       // Cluster Number of the Start of the Root Directory
    printf("FSInfo: %" PRIu16 "\n", BS->FSInfo);           // Sector Number of the FileSystem Information Sector
    printf("BkBootSec: %" PRIu16 "\n", BS->BkBootSec);     // Sector Number of the BackupBoot Sector
    printf("Reserved: ");
    for (int i = 0; i < 12; i++) // Reserved
        printf("%" PRIu8, BS->reserved[i]);
    printf("\n");
    printf("DrvNum: 0x%X\n", BS->DrvNum);             // Logical Drive Number ofPartition
    printf("Reserved1: %" PRIu8 "\n", BS->Reserved1); // Unused
    printf("BootSig: 0x%X\n", BS->BootSig);           // Extended Signature
    printf("VolID: %" PRIu32 "\n", BS->VolID);        // Serial Number of Partition
    printf("Vol Partition: ");
    for (int i = 0; i < 11; i++) // Volume Name of Partition
        printf("%c", BS->VolLab[i]);
    printf("\n");
    printf("FilSysType: ");
    for (int i = 0; i < 8; i++) // FAT Name (FAT32)
        printf("%c", BS->FilSysType[i]);
    printf("\n");
    printf("SigA: 0x%X\n", BS->SigA); // Boot Record Signature A
    printf("SigB: 0x%X\n", BS->SigB); // Boot Record Signature B
}

void check_sig()
{
    if ((BS->SigA != 0x55) || (BS->SigB != 0xAA))
    {
        printf("Invalid boot sector signature!\n");
        exit(-1);
    }
}

void set_BS(int fd)
{
    BS = (boot_sector *)malloc(sizeof(boot_sector));
    if (BS == NULL)
    {
        printf("Error malloc\n");
        exit(0);
    }

    if (read(fd, BS, 512) == -1)
        perror("read");

    check_sig();
};
