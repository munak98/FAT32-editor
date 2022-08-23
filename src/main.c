#include "../headers/boot_sector.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int main()
{
    int count;
    int fd; // File reading from
    unsigned char *buffer;
    boot_sector *bootSector;

    buffer = (unsigned char *)malloc(sizeof(unsigned char) * 512);
    bootSector = (boot_sector *)malloc(sizeof(boot_sector));
    if (buffer == NULL || bootSector == NULL)
    {
        printf("Error malloc\n");
        exit(0);
    }

    // opening flash drive file
    fd = open("/dev/sdb", O_RDWR);
    if (fd == -1)
        perror("open");

    // reading the first 512 bytes (boot sector) to buffer
    count = read(fd, buffer, 512);
    if (count == -1)
        perror("read");

    // copying the buffer content to the struct boot_sector, which has all boot sector fields defined
    memcpy(bootSector, buffer, sizeof(boot_sector));

    show_device_info(bootSector);

    free(buffer);
    free(bootSector);

    return 0;
}