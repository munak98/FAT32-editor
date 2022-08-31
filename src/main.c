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
    char cmd[11];
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

    init_DirEntry();

    while (1)
    {
        printf("%s -> ", currDirEntry->Name);
        printf("not?\n");

        scanf("%s", cmd);
        printf("cmmd %s\n", cmd);

        exec(fd, cmd);
        printf("problem?\n");
    }

    free(BS);
    free(FAT32);
    free(currDirEntry);
    return 0;
}