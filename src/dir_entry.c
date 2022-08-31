#include "../headers/boot_sector.h"
#include "../headers/dir_entry.h"
#include "../headers/FAT.h"
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

    fatherDirEntry = (dir_entry *)malloc(sizeof(dir_entry));
    *fatherDirEntry = *currDirEntry;
}

void show_entry(dir_entry *entry)
{
    int dir, volum_label;
    dir = entry->Attr & AttrDirectory;
    volum_label = entry->Attr & AttrVolumeLabel;
    if (dir)
        printf("%-11s\t %-12s\n", entry->Name, "DIR");
    else if (volum_label)
        printf("%-11s\t %-12s\n", entry->Name, "VOL LABEL");
    else
        printf("%-11s\t %-12s\n", entry->Name, "FILE");
}