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
    currDirEntry = (dir_entry *)malloc(sizeof(dir_entry));

    memcpy(currDirEntry->Name, &root, 11);
    currDirEntry->FstClusHI = BS->RootClus >> 8;
    currDirEntry->FstClusLO = BS->RootClus & 0xFF;

    rootDirEntry = (dir_entry *)malloc(sizeof(dir_entry));
    fatherDirEntry = (dir_entry *)malloc(sizeof(dir_entry));
    *fatherDirEntry = *currDirEntry;
    *rootDirEntry = *currDirEntry;
}

int size(uint8_t *name, int max_size)
{
    int i = 0;
    while (name[i] != 255 && name[i] != 0 && (i < max_size))
    {
        i += 2;
    }
    return i;
}

int copy_field(uint8_t *field, int max_size, uint8_t *long_name)
{
    int s = size(field, max_size);
    memcpy(long_name, field, s);
    return s;
}

int copy_name_fields(long_dir_entry *longDirEntry, uint8_t *long_name)
{
    int used = 0;
    used += copy_field(longDirEntry->LDIR_Name1, 10, long_name);
    long_name = realloc(long_name, used + 12);
    used += copy_field(longDirEntry->LDIR_Name2, 12, &long_name[used]);
    long_name = realloc(long_name, used + 4);
    used += copy_field(longDirEntry->LDIR_Name3, 4, &long_name[used]);
    long_name = realloc(long_name, used);
    long_name[used] = '\0';
    return used;
}

uint8_t *get_short_name(dir_entry *dirEntry)
{
    int i = 0;
    for (i; i < 11; i++)
    {
        if (dirEntry->Name[i] == ' ')
            break;
    }

    uint8_t *short_name = malloc(sizeof(uint8_t) * (i + 1));
    for (int j = 0; j < i; j++)
    {
        short_name[j] = dirEntry->Name[j];
    }

    short_name[i + 1] = '\0';
    return short_name;
}

void show_entry(dir_entry *entry, uint8_t **long_name)
{
    if (is_dir(entry))
        printf("\t%-12s%-20.11s", "DIR", entry->Name);

    else if (is_volum(entry))
        printf("\t%-12s%-20.11s", "VOLUME", entry->Name);

    else
        printf("\t%-12s%-20.11s", "FILE", entry->Name);

    if (long_name)
        print_long_name(long_name);

    printf("\n");
}

void print_name_part(uint8_t *name)
{
    int i = 0;
    while (name[i] != '\0')
    {
        printf("%lc", name[i]);
        i += 2;
    }
}

void print_long_name(uint8_t **name)
{
    int i = 0;
    while (name[i] != NULL)
    {
        print_name_part(name[i]);
        i++;
    }
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