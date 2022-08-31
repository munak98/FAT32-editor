#include "../headers/dir_entry.h"
#include "../headers/FAT.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

void set_FAT(int fd)
{
    FAT32 = (uint32_t *)malloc(BS->FATSz32 * BS->BytesPerSec);
    if (FAT32 == NULL)
    {
        printf("Error malloc\n");
        exit(0);
    }
    // jumping reserved sectors and reading the FAT into FAT32 variable
    lseek(fd, BS->RsvdSecCnt * BS->BytesPerSec, SEEK_SET);
    if (read(fd, FAT32, BS->FATSz32 * BS->BytesPerSec) == -1)
    {
        perror("read");
        exit(-1);
    }
}

void show_FAT(int until_idx)
{
    for (int i = 0; i < until_idx; i++)
    {
        printf("[%02d]: 0x%08X  ", i, FAT32[i]);
        if (i % 5 == 4)
            printf("\n");
    }
}

void set_first_data_sector()
{
    first_data_sector = BS->RsvdSecCnt + (BS->NumFATs * BS->FATSz32);
}

int get_first_sec_of_clus(int n)
{
    return ((n - 2) * BS->SecPerClus) + first_data_sector;
}

int next_byte_zr(int fd)
{
    int8_t next_byte;
    if (read(fd, &next_byte, 1) == -1)
        perror("name read");
    lseek(fd, -1, SEEK_CUR);
    if (next_byte == 0)
        return 1;
    else
        return 0;
}

void read_entries(int n, int fd, int count)
{
    int8_t buffer[DIR_ENTRY_SIZE];
    int long_file, dir, pos;
    currDirEntry = (dir_entry *)malloc(sizeof(dir_entry));
    if (currDirEntry == NULL)
    {
        printf("Error malloc\n");
        exit(0);
    }

    lseek(fd, get_first_sec_of_clus(n) * BS->BytesPerSec, SEEK_SET);
    while (1)
    {
        if (read(fd, &buffer, DIR_ENTRY_SIZE) < 1)
            perror("dir entry read");

        if (buffer[0] == 0x00)
            break;

        dir = buffer[0x0B] & AttrDirectory;
        long_file = (buffer[0x0B] & (AttrReadOnly | AttrHidden | AttrSystem | AttrVolumeLabel)) == 0x0F;

        if (long_file)
        {
            continue;
        }
        else
        {
            memcpy(currDirEntry, buffer, sizeof(dir_entry));
            for (int i = 0; i < count; i++)
            {
                printf("    ");
            }
            if (count > 0)
            {
                printf(" |");
            }

            for (int i = 0; i < count; i++)
            {
                printf("__");
            }
            printf(" %s\n", currDirEntry->Name);

            if (dir && currDirEntry->Name[0] != '.')
            {
                pos = lseek(fd, 0, SEEK_CUR);
                read_entries(currDirEntry->FstClusHI << 8 | currDirEntry->FstClusLO, fd, count + 1);
                lseek(fd, pos, SEEK_SET);
            }
        }
    }
    return;
}

void traverse_device(int fd)
{
    printf("All files in the device: \n");
    read_entries(BS->RootClus, fd, 0);
}