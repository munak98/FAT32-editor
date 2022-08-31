#include "../headers/boot_sector.h"
#include "../headers/dir_entry.h"
#include "../headers/FAT.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

char *builtin_str[] = {
    "cd",
    "ls",
    "cat",
};

void (*builtin_func[])(int) = {
    &cd,
    &ls,
    &cat,
};

void set_FAT(int fd)
{
    FAT32 = (uint32_t *)malloc(BS->FATSz32 * BS->BytesPerSec);
    if (FAT32 == NULL)
    {
        printf("Error malloc\n");
        exit(0);
    }
    lseek(fd, BS->RsvdSecCnt * BS->BytesPerSec, SEEK_SET);
    if (read(fd, FAT32, BS->FATSz32 * BS->BytesPerSec) == -1)
    {
        perror("read");
        exit(-1);
    }
}

void show_FAT(int until_idx)
{
    for (int i = 0; i < until_idx; i++)
    {
        printf("[%02d]: 0x%08X  ", i, FAT32[i]);
        if (i % 5 == 4)
            printf("\n");
    }
}

int get_first_sec_of_clus(int n)
{
    return ((n - 2) * BS->SecPerClus) + (BS->RsvdSecCnt + (BS->NumFATs * BS->FATSz32));
}

void get_file_content(int fd, int init_clu, int file_size, char *content)
{
    u_int32_t curr_clu = init_clu;
    int nbytes = BS->BytesPerSec;
    int bytes_read = 0;
    int sectors_read = 0;
    u_int8_t buffer[BS->BytesPerSec];

    while (1) // for each cluster
    {
        lseek(fd, get_first_sec_of_clus(curr_clu) * BS->BytesPerSec, SEEK_SET);
        sectors_read = 0;
        while (1) // for each sector
        {

            if (bytes_read + BS->BytesPerSec > file_size)
            {
                nbytes = file_size % BS->BytesPerSec;
            }

            if (read(fd, &buffer, nbytes) < 1)
                perror("file sector read");

            memcpy(&content[bytes_read], buffer, nbytes);

            bytes_read += nbytes;
            sectors_read++;
            if (sectors_read == BS->SecPerClus || bytes_read == file_size)
                break;
        }

        if ((curr_clu = FAT32[curr_clu]) == FAT32[1])
            break;
    }
}

void print_identation(int count)
{
    for (int i = 0; i < count; i++)
    {
        printf("    ");
    }
    if (count > 0)
    {
        printf(" |");
    }

    for (int i = 0; i < count; i++)
    {
        printf("__");
    }
}

// TO DO: passar um nome como arg com espaço em branco
// ou printar o nome dos arquivos reais usando long file name
dir_entry *look_up_entry(int fd, u_int8_t flags)
{
    dir_entry *auxDirEntry;
    auxDirEntry = (dir_entry *)malloc(sizeof(dir_entry));

    int8_t buffer[DIR_ENTRY_SIZE];
    int is_long_file, is_dir;
    dir_entry curr_dir;
    char name[11];

    lseek(fd, get_first_sec_of_clus(currDirEntry->FstClusHI << 8 | currDirEntry->FstClusLO) * BS->BytesPerSec, SEEK_SET);

    if (flags & NeedsName)
    {
        fgets(name, sizeof(name), stdin);
    }
    while (1)
    {
        if (read(fd, &buffer, DIR_ENTRY_SIZE) < 1)
            perror("read dir entry failure");

        if (buffer[0] == 0x00)
        {
            return NULL;
        }

        is_dir = buffer[0x0B] & AttrDirectory;
        is_long_file = (buffer[0x0B] & (AttrReadOnly | AttrHidden | AttrSystem | AttrVolumeLabel)) == 0x0F;

        if (is_long_file)
            continue;
        else
        {
            memcpy(auxDirEntry, buffer, sizeof(dir_entry));
            if (flags & DirEntries) // ls command
            {
                show_entry(auxDirEntry);
            }
            else
            {
                // for (size_t i = 0; i < strlen(name) - 1; i++)
                // {
                //     printf("%c %c\n", buffer[i], name[i + 1]);
                // }
                if (memcmp(buffer, &name[1], strlen(name) - 1) == 0) // cd and cat commands
                    return auxDirEntry;
            }
        }
    }
    free(auxDirEntry);
}

void cat(int fd)
{
    dir_entry *auxDirEntry;
    auxDirEntry = (dir_entry *)malloc(sizeof(dir_entry));
    auxDirEntry = look_up_entry(fd, 0x4);
    if (auxDirEntry == NULL)
    {
        printf("No such file in directory\n");
        return;
    }

    char *content = malloc(sizeof(char) * auxDirEntry->FileSize);
    get_file_content(fd, auxDirEntry->FstClusHI << 8 | auxDirEntry->FstClusLO, auxDirEntry->FileSize, content);
    printf("%s", content);
    free(content);
    free(auxDirEntry);
    return;
}

void ls(int fd)
{
    look_up_entry(fd, 0x1);
}

void cd(int fd)
{
    dir_entry *auxDirEntry;
    auxDirEntry = (dir_entry *)malloc(sizeof(dir_entry));
    auxDirEntry = look_up_entry(fd, 0x4);
    if (auxDirEntry == NULL)
    {
        printf("No such directory\n");
        return;
    }
    if (auxDirEntry->Attr & AttrDirectory)
    {
        *currDirEntry = *auxDirEntry;
        return;
    }
    else
    {
        printf("File is not a directory\n");
        return;
    }
    free(auxDirEntry);
}

void exec(int fd, char *cmd)
{
    for (size_t i = 0; i < 4; i++)
    {
        if (strcmp(cmd, builtin_str[i]) == 0)
        {
            builtin_func[i](fd);
            return;
        }
    }
    printf("command not found\n");
    return;
}