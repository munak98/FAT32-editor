#include "../headers/boot_sector.h"
#include "../headers/dir_entry.h"
#include "../headers/FAT.h"
#include "../headers/utils.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

void init_DirEntry()
{
    char root[11] = "home";
    rootDirEntry = (dir_entry *)malloc(sizeof(dir_entry));
    rootDirEntry->FstClusHI = BS->RootClus >> 8;
    rootDirEntry->FstClusLO = BS->RootClus & 0xFF;
    memcpy(rootDirEntry->Name, &root, 11);
    rootDirEntry->LongName = NULL;

    currDirEntry = (dir_entry *)malloc(sizeof(dir_entry));

    fatherDirEntry = (dir_entry *)malloc(sizeof(dir_entry));

    *currDirEntry = *rootDirEntry;

    *fatherDirEntry = *rootDirEntry;
}

char *get_short_name(dir_entry *dirEntry)
{
    int i = 0;
    for (i; i < 11; i++)
    {
        if (dirEntry->Name[i] == ' ')
            break;
    }

    char *short_name = malloc(sizeof(char) * (i + 1));
    for (int j = 0; j < i; j++)
    {
        short_name[j] = dirEntry->Name[j];
    }

    short_name[i + 1] = '\0';
    return short_name;
}

int copy_name_fields(long_dir_entry *longDirEntry, uint16_t *long_filename)
{
    char *p = memccpy(long_filename, longDirEntry->LDIR_Name1, 0xFFFF, 5 * sizeof(uint16_t));

    if (p == NULL)
        p = memccpy(&long_filename[5], longDirEntry->LDIR_Name2, 0xFFFF, 6 * sizeof(uint16_t));
    if (p == NULL)
        p = memccpy(&long_filename[11], longDirEntry->LDIR_Name3, 0xFFFF, 2 * sizeof(uint16_t));
    if (p == NULL)
        return 1;
    else
        return 0;
}

void write_name_fields(uint16_t *shifted_name, int fd)
{
    lseek(fd, -DIR_ENTRY_SIZE + 1, SEEK_CUR);
    if (write(fd, shifted_name, 5 * sizeof(uint16_t)) < 1)
        perror("delete entry failure");

    lseek(fd, 3, SEEK_CUR);
    if (write(fd, &shifted_name[5], 6 * sizeof(uint16_t)) < 1)
        perror("delete entry failure");
    lseek(fd, 2, SEEK_CUR);
    if (write(fd, &shifted_name[11], 2 * sizeof(uint16_t)) < 1)
        perror("delete entry failure");
}

void print_long_name(uint16_t *name)
{
    int i = 0;
    if (!name)
        printf("long name null\n");
    while (name[i] != 0)
    {
        printf("%lc", name[i]);
        i++;
    }
}

void show_entry(dir_entry *entry, uint16_t *long_name, int has_long_name)
{
    if (is_volum(entry))
        return;
    if (is_dir(entry))
        printf("\t%-12s", "DIR");

    else
        printf("\t%-12s", "FILE");

    if (has_long_name)
        print_long_name(long_name);
    else
        printf("%s ", entry->Name);

    printf("\n");
}

int is_long_file(uint8_t flag)
{
    return (flag & (AttrReadOnly | AttrHidden | AttrSystem | AttrVolumeLabel)) == 0x0F;
}

int is_dir(dir_entry *entry)
{
    return ((entry->Attr & AttrDirectory) == AttrDirectory);
}

int is_volum(dir_entry *entry)
{
    return ((entry->Attr & AttrVolumeLabel) == AttrVolumeLabel);
}

int is_hidden(uint16_t first_cha)
{
    return (first_cha == '.');
}

int is_system(uint8_t flag)
{
    return ((flag & AttrSystem) == AttrSystem);
}

int is_deleted(uint8_t flag)
{
    return flag == 0xE5;
}

uint16_t first_clu(dir_entry *entry)
{
    return (entry->FstClusHI << 8 | entry->FstClusLO);
}
