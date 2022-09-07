#include <inttypes.h>

#ifndef FAT_H
#define FAT_H

#define DIR_ENTRY_LAST_AND_UNUSED 0x0
#define DIR_ENTRY_UNUSED 0xE5

uint32_t *FAT32;

void set_FAT();
void show_FAT(int);

int get_first_sec_of_clus(int);

void cd(int, char *);
void ls(int, char *);
void cat(int, char *);
void undelete(int, char *);

void exec(int, char **);

#endif