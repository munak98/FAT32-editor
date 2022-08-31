#include <inttypes.h>
#include "../headers/boot_sector.h"

#ifndef FAT_H
#define FAT_H

uint32_t *FAT32;

int first_data_sector;

void set_FAT();
void show_FAT(int);

void set_first_data_sector();
int get_first_sec_of_clus(int);

void read_entries(int, int, int);

void follow_chain(int);

void traverse_device(int);
#endif