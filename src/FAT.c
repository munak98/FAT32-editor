#include "../headers/FAT.h"
#include <stdio.h>

void show_FAT(int until_idx, uint32_t *FAT)
{
    for (int i = 0; i < until_idx; i++)
    {
        printf("[%02d]: 0x%08X  ", i, FAT[i]);
        if (i % 5 == 4)
            printf("\n");
    }
}
