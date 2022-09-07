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

    init_DirEntry();
    char *cmd = NULL;
    char **tokens = NULL;

    while (1)
    {
        show_prompt();
        cmd = read_line();
        tokens = split(cmd, " ");
        exec(fd, tokens);
    }

    free(BS);
    free(currDirEntry);
    return 0;
}

// TODO: entrar com comandos usando readline e perguntar o nome do arquivo