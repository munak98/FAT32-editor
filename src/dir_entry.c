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

    rootDirEntry = (dir_entry *)malloc(sizeof(dir_entry));
    fatherDirEntry = (dir_entry *)malloc(sizeof(dir_entry));
    *fatherDirEntry = *currDirEntry;
    *rootDirEntry = *currDirEntry;
}

void show_entry(dir_entry *entry, uint8_t *long_name, int flag)
{
    int dir, volum_label;
    dir = entry->Attr & AttrDirectory;
    volum_label = entry->Attr & AttrVolumeLabel;
    if (dir)
    {

        printf("\t%-12s%-20s", "DIR", entry->Name);
    }
    else if (volum_label)
    {

        printf("\t%-12s%-20s", "VOLUME", entry->Name);
    }
    else
    {
        printf("\t%-12s%-20s", "FILE", entry->Name);
    }

    if (flag)
    {
        for (int i = 0; i < 26; i++)
        {
            printf("%lc", long_name[i]);
        }
    }
    printf("\n");
}
