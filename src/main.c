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

    // opening flash drive file
    fd = open("/dev/sdb", O_RDWR);
    if (fd == -1)
    {
        perror("open");
        exit(-1);
    }

    // reading and structuring the boot sector
    set_BS(fd);

    // reading the FAT
    set_FAT(fd);

    // print the first 20 entries on FAT
    show_FAT(20);

    set_first_data_sector();

    traverse_device(fd);

    free(BS);
    free(FAT32);
    free(currDirEntry);
    return 0;
}