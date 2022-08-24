#include "../headers/boot_sector.h"
#include "../headers/dir_entry.h"
#include "../headers/FAT.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int main()
{
    int fd; // File reading from
    int FirstDataSector;
    int FirstSectorofCluster;

    boot_sector *bootSector = (boot_sector *)malloc(sizeof(boot_sector));
    dir_entry *dirEntry = (dir_entry *)malloc(sizeof(dir_entry));

    if (bootSector == NULL || dirEntry == NULL)
    {
        printf("Error malloc\n");
        exit(0);
    }

    // opening flash drive file
    fd = open("/dev/sdb", O_RDWR);
    if (fd == -1)
        perror("open");

    // reading and structuring the boot sector in bootSector variable
    read_boot_sector(fd, bootSector);

    // variable to hold the FAT table
    uint32_t *FAT32 = (uint32_t *)malloc(bootSector->FATSz32 * bootSector->BytesPerSec);

    // jumping reserved sectors and reading the FAT into FAT32 variable
    lseek(fd, bootSector->RsvdSecCnt * bootSector->BytesPerSec, SEEK_SET);
    if (read(fd, FAT32, bootSector->FATSz32 * bootSector->BytesPerSec) == -1)
        perror("read");

    // print the first 20 entries on FAT table
    show_FAT(20, FAT32);

    FirstDataSector = bootSector->RsvdSecCnt + (bootSector->NumFATs * bootSector->FATSz32);
    FirstSectorofCluster = ((bootSector->RootClus - 2) * bootSector->SecPerClus) +
                           FirstDataSector;

    // jumping to the first sector of the Root Cluster, computed above
    lseek(fd, FirstSectorofCluster * bootSector->BytesPerSec, SEEK_SET);

    // reading the Root Cluster first sector in chunks of 32 bytes (sector size = 512 bytes),
    // structuring them into a dir_entry variable and printing the entry name
    for (int i = 0; i < 16; i++)
    {
        lseek(fd, 32 * i, SEEK_CUR);
        if (read(fd, dirEntry, 32) == -1)
            perror("read");
        if (*dirEntry->Name == 0)
            break;
        printf("-> %s\n", dirEntry->Name);
    }

    free(bootSector);
    free(dirEntry);
    free(FAT32);
    return 0;
}