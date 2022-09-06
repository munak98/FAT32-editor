#include <inttypes.h>

#ifndef FAT_H
#define FAT_H

uint32_t *FAT32;

void set_FAT();
void show_FAT(int);

int get_first_sec_of_clus(int);

void cd(int, char *);
void ls(int, char *);
void cat(int, char *);

enum Flags
{
    DirEntries = 0x1,
    NeedsName = 0x4,
};

void exec(int, char **);

#endif