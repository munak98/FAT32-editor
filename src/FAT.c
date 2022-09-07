#include "../headers/boot_sector.h"
#include "../headers/dir_entry.h"
#include "../headers/FAT.h"
#include <fcntl.h>
#include "../headers/utils.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

char *builtin_str[] = {
    "cd",
    "ls",
    "cat",
    "undelete",
};

void (*builtin_func[])(int, char *) = {
    &cd,
    &ls,
    &cat,
    &undelete,
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

uint32_t get_next_clu(int fd, int curr)
{
    uint32_t next_clu;
    lseek(fd, (BS->RsvdSecCnt * BS->BytesPerSec) + (curr * 4), SEEK_SET);
    if (read(fd, &next_clu, 4) == -1)
    {
        perror("read");
        exit(-1);
    }
    return next_clu;
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

        if ((curr_clu = get_next_clu(fd, curr_clu)) == get_next_clu(fd, 1))
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

dir_entry *get_entry(int fd, char *arg)
{
    dir_entry *auxDirEntry = malloc(sizeof(dir_entry));
    uint8_t buffer[DIR_ENTRY_SIZE];
    char *short_name;

    lseek(fd, get_first_sec_of_clus(currDirEntry->FstClusHI << 8 | currDirEntry->FstClusLO) * BS->BytesPerSec, SEEK_SET);

    while (1)
    {
        if (read(fd, &buffer, DIR_ENTRY_SIZE) < 1)
            perror("read dir entry failure");

        if (buffer[0] == 0x00)
        {
            free(auxDirEntry);
            return NULL;
        }

        if (is_long_file(buffer[0x0B]))
            continue;
        else
        {
            memcpy(auxDirEntry, buffer, sizeof(buffer));
            short_name = get_short_name(auxDirEntry);
            if (strcmp(short_name, arg) == 0)
                return auxDirEntry;
        }
    }
}

void undelete(int fd, char *args)
{
    ;
}

void cat(int fd, char *arg)
{
    dir_entry *auxDirEntry;
    auxDirEntry = (dir_entry *)malloc(sizeof(dir_entry));
    auxDirEntry = get_entry(fd, arg);
    if (!auxDirEntry)
    {
        printf("[error] no such file in directory\n");
        return;
    }
    if (is_dir(auxDirEntry))
    {
        printf("[error] name is a directory, not a file\n");
        return;
    }

    char *content = malloc(sizeof(char) * auxDirEntry->FileSize);
    get_file_content(fd, auxDirEntry->FstClusHI << 8 | auxDirEntry->FstClusLO, auxDirEntry->FileSize, content);
    printf("%s", content);
    free(content);
    free(auxDirEntry);
    return;
}

void ls(int fd, char *arg)
{

    dir_entry *auxDirEntry = (dir_entry *)malloc(sizeof(dir_entry));
    long_dir_entry *longDirEntry = malloc(sizeof(long_dir_entry));

    uint8_t buffer[DIR_ENTRY_SIZE];
    uint8_t **long_name_parts = NULL;
    lseek(fd, get_first_sec_of_clus(currDirEntry->FstClusHI << 8 | currDirEntry->FstClusLO) * BS->BytesPerSec, SEEK_SET);

    printf("\t%-12s%-20s%-10s\t\n", "File type", "Short name", "Long name");
    printf("\t===========================================\n");
    while (1)
    {
        if (read(fd, buffer, DIR_ENTRY_SIZE) < 1)
            perror("read dir entry failure");

        if (buffer[0] == 0x00)
            return;

        if (is_long_file(buffer[0x0B]))
        {
            memcpy(longDirEntry, buffer, sizeof(long_dir_entry));
            if (longDirEntry->LDIR_Ord == 0xE5)
                continue;

            int8_t seq_number = longDirEntry->LDIR_Ord - 0x40;
            long_name_parts = malloc(sizeof(uint8_t *) * (seq_number + 1));
            long_name_parts[seq_number] = NULL;

            uint8_t *long_name = malloc(SIZE_LONG_NAME);
            copy_name_fields(longDirEntry, long_name);
            long_name_parts[seq_number - 1] = long_name;

            int8_t seq_size = seq_number - 0x1;
            while (1)
            {
                if (seq_size == 0x0)
                    break;
                if (read(fd, buffer, DIR_ENTRY_SIZE) < 1)
                    perror("read dir entry failure");
                memcpy(longDirEntry, buffer, sizeof(long_dir_entry));
                seq_number = longDirEntry->LDIR_Ord;

                uint8_t *long_name = malloc(SIZE_LONG_NAME);
                copy_name_fields(longDirEntry, long_name);
                long_name_parts[seq_number - 1] = long_name;
                seq_size = seq_number - 0x1;
            }
        }
        else
        {
            memcpy(auxDirEntry, buffer, sizeof(buffer));
            if (auxDirEntry->Name[0] == 0xE5)
                continue;

            show_entry(auxDirEntry, long_name_parts);
            if (long_name_parts)
            {
                free_content(long_name_parts);
                free(long_name_parts);
                long_name_parts = NULL;
            }
        }
    }
    free(auxDirEntry);
    free(longDirEntry);
}

dir_entry *get_entry_from_dot_entry(int fd, dir_entry *entry)
{
    if (!entry)
        return NULL;
    dir_entry *auxDirEntry = malloc(sizeof(dir_entry));
    uint8_t buffer[DIR_ENTRY_SIZE];
    char *short_name;

    lseek(fd, get_first_sec_of_clus(rootDirEntry->FstClusHI << 8 | rootDirEntry->FstClusLO) * BS->BytesPerSec, SEEK_SET);

    while (1)
    {
        if (read(fd, &buffer, DIR_ENTRY_SIZE) < 1)
            perror("read dir entry failure");

        if (buffer[0] == 0x00)
        {
            free(auxDirEntry);
            return NULL;
        }

        if (is_long_file(buffer[0x0B]))
            continue;
        else
        {
            memcpy(auxDirEntry, buffer, sizeof(buffer));
            if (auxDirEntry->Name[0] != '.' && memcmp(auxDirEntry->Name, "Pen", 3) != 0)
                if ((auxDirEntry->FstClusHI << 8 | auxDirEntry->FstClusLO) == (entry->FstClusHI << 8 | entry->FstClusLO))
                    return auxDirEntry;
        }
    }
}

void cd(int fd, char *arg)
{
    if (strcmp(arg, ".") == 0)
    {
        return;
    }
    dir_entry *auxDirEntry;
    if (strcmp(arg, "..") == 0)
    {
        free(currDirEntry);

        currDirEntry = fatherDirEntry;
        auxDirEntry = get_entry(fd, "..");
        fatherDirEntry = get_entry_from_dot_entry(fd, auxDirEntry);

        if (!fatherDirEntry)
            fatherDirEntry = rootDirEntry;

        free(auxDirEntry);
        return;
    }

    auxDirEntry = get_entry(fd, arg);
    if (!auxDirEntry)
        printf("No such directory\n");
    else if (is_dir(auxDirEntry))
    {
        *fatherDirEntry = *currDirEntry;
        *currDirEntry = *auxDirEntry;
    }
    else
        printf("File is not a directory\n");
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