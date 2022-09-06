#include "../headers/boot_sector.h"
#include "../headers/dir_entry.h"
#include "../headers/FAT.h"
#include <fcntl.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

char *builtin_str[] = {
    "cd",
    "ls",
    "cat",
};

void (*builtin_func[])(int, char *) = {
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

// TODO: consertar bug no cat
dir_entry *look_up_entry(int fd, char *arg, u_int8_t flags)
{
    dir_entry *auxDirEntry;
    auxDirEntry = (dir_entry *)malloc(sizeof(dir_entry));

    int8_t buffer[DIR_ENTRY_SIZE];
    long_dir_entry *longDirEntry = malloc(sizeof(long_dir_entry));
    uint8_t *long_name = malloc(sizeof(uint8_t) * 26);
    int is_long_file;
    int holding_long_name = 0;

    lseek(fd, get_first_sec_of_clus(currDirEntry->FstClusHI << 8 | currDirEntry->FstClusLO) * BS->BytesPerSec, SEEK_SET);

    while (1)
    {
        if (read(fd, &buffer, DIR_ENTRY_SIZE) < 1)
            perror("read dir entry failure");

        if (buffer[0] == 0x00)
        {
            return NULL;
        }

        is_long_file = (buffer[0x0B] & (AttrReadOnly | AttrHidden | AttrSystem | AttrVolumeLabel)) == 0x0F;

        if (is_long_file)
        {
            holding_long_name = 1;

            memcpy(longDirEntry, buffer, sizeof(long_dir_entry));
            memcpy(long_name, longDirEntry->LDIR_Name1, 10);
            memcpy(&long_name[10], longDirEntry->LDIR_Name2, 12);
            memcpy(&long_name[22], longDirEntry->LDIR_Name3, 4);
        }
        else
        {
            memcpy(auxDirEntry, buffer, sizeof(dir_entry));
            if (flags & DirEntries) // ls command
            {
                show_entry(auxDirEntry, long_name, holding_long_name);
            }
            else
            {
                if (memcmp(buffer, arg, strlen(arg)) == 0) // cd and cat commands
                    return auxDirEntry;
            }
            holding_long_name = 0;
        }
    }
    free(auxDirEntry);
}

void cat(int fd, char *arg)
{
    dir_entry *auxDirEntry;
    auxDirEntry = (dir_entry *)malloc(sizeof(dir_entry));
    auxDirEntry = look_up_entry(fd, arg, 0x4);
    if (auxDirEntry == NULL)
    {
        printf("[error] no such file in directory\n");
        return;
    }
    if (auxDirEntry->Attr & AttrDirectory)
    {
        printf("[error] name is a directory, not a file\n");
        return;
    }

    char *content = malloc(sizeof(char) * auxDirEntry->FileSize);
    get_file_content(fd, auxDirEntry->FstClusHI << 8 | auxDirEntry->FstClusLO, auxDirEntry->FileSize, content);
    printf("%s\n", content);
    free(content);
    free(auxDirEntry);
    return;
}

void ls(int fd, char *arg)
{
    printf("\t%-12s%-20s%-10s\t\n", "File type", "Short name", "Long name");
    printf("\t===========================================\n");
    look_up_entry(fd, arg, 0x1);
}

void cd(int fd, char *arg)
{
    if (strcmp(arg, ".") == 0)
    {
        return;
    }
    if (strcmp(arg, "..") == 0)
    {
        *currDirEntry = *fatherDirEntry;
        fatherDirEntry = look_up_entry(fd, currDirEntry->Name, 0x4);
        if (fatherDirEntry == NULL)
        {
            fatherDirEntry = rootDirEntry;
        }
        return;
    }
    dir_entry *auxDirEntry;
    auxDirEntry = (dir_entry *)malloc(sizeof(dir_entry));
    auxDirEntry = look_up_entry(fd, arg, 0x4);
    if (auxDirEntry == NULL)
    {
        printf("No such directory\n");
    }
    else if (auxDirEntry->Attr & AttrDirectory)
    {
        *fatherDirEntry = *currDirEntry;
        *currDirEntry = *auxDirEntry;
    }
    else
    {
        printf("File is not a directory\n");
    }
    free(auxDirEntry);
    return;
}

void exec(int fd, char **tokens)
{
    char *cmd = tokens[0];
    char *args = tokens[1];

    for (size_t i = 0; i < 4; i++)
    {
        if (strcmp(cmd, builtin_str[i]) == 0)
        {
            builtin_func[i](fd, args);
            return;
        }
    }
    printf("command not found\n");
    return;
}