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
    "hide",
    "unhide",

};

void (*builtin_func[])(int, char *) = {
    &cd,
    &ls,
    &cat,
    &undelete,
    &hide,
    &unhide,

};

int first_sec(int n)
{
    return ((n - 2) * BS->SecPerClus) + (BS->RsvdSecCnt + (BS->NumFATs * BS->FATSz32));
}

uint32_t next_clu(int fd, int curr)
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

char *get_file_content(int fd, int init_clu, int file_size)
{
    u_int32_t curr_clu = init_clu;
    char *content = malloc(sizeof(char) * file_size);
    int nbytes = BS->BytesPerSec;
    int bytes_read = 0;
    int sectors_read = 0;
    u_int8_t buffer[BS->BytesPerSec];

    while (1) // for each cluster
    {
        lseek(fd, first_sec(curr_clu) * BS->BytesPerSec, SEEK_SET);
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

        if ((curr_clu = next_clu(fd, curr_clu)) == next_clu(fd, 1))
            break;
    }

    return content;
}

void hide(int fd, char *arg)
{
    ;
}

void unhide(int fd, char *arg)
{
    ;
}

void undelete(int fd, char *arg)
{
    dir_entry *auxDirEntry = malloc(sizeof(dir_entry));

    uint8_t buffer[DIR_ENTRY_SIZE];
    char restore = 'A';

    lseek(fd, first_sec(first_clu(currDirEntry)) * BS->BytesPerSec, SEEK_SET);

    while (1)
    {
        if (read(fd, &buffer, DIR_ENTRY_SIZE) < 1)
            perror("read dir entry failure");

        if (buffer[0] == 0x00)
        {
            free(auxDirEntry);
            printf("returning because 0x00\n");
            return;
        }

        if (is_long_file(buffer[0x0B]))
        {
            if (is_deleted(buffer[0]))
            {
                lseek(fd, -DIR_ENTRY_SIZE, SEEK_CUR);
                if (write(fd, &restore, 1) < 1)
                    perror("undelete entry failure");
                lseek(fd, DIR_ENTRY_SIZE - 1, SEEK_CUR);
                printf("undeleted long entry\n");
            }
        }
        else
        {
            if (is_deleted(buffer[0]))
            {
                lseek(fd, -DIR_ENTRY_SIZE, SEEK_CUR);
                if (write(fd, &restore, 1) < 1)
                    perror("undelete entry failure");
                lseek(fd, DIR_ENTRY_SIZE - 1, SEEK_CUR);
                printf("undeleted short entry\n");
                return;
            }
        }
    }
}

dir_entry *get_entry(int fd, char *arg)
{
    dir_entry *auxDirEntry = malloc(sizeof(dir_entry));
    uint8_t buffer[DIR_ENTRY_SIZE];
    long_dir_entry *longDirEntry = malloc(sizeof(long_dir_entry));
    int8_t seq_number;

    uint16_t long_filename[256];
    int multp = 0;
    char *short_name;
    int has_long_name = 0;


    lseek(fd, first_sec(first_clu(currDirEntry)) * BS->BytesPerSec, SEEK_SET);

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
        {
            if (buffer[0] != 0xE5)
            {
                has_long_name = 1;
                memcpy(longDirEntry, buffer, sizeof(long_dir_entry));

                if ((longDirEntry->LDIR_Ord & 0x40) == 0x40)
                    seq_number = longDirEntry->LDIR_Ord - 0x40;
                else
                    seq_number = longDirEntry->LDIR_Ord;

                multp = copy_name_fields(longDirEntry, &long_filename[13 * (seq_number - 1)]);
                if ((longDirEntry->LDIR_Ord & 0x40) && multp)
                    long_filename[13 * seq_number] = 0x0000;
            }
        }
        else
        {
            memcpy(auxDirEntry, buffer, sizeof(buffer));
            if (has_long_name)
            {
                if (is_equal(arg, long_filename))
                {
                    auxDirEntry->LongName = malloc(sizeof(uint16_t) * 256);
                    memcpy(auxDirEntry->LongName, long_filename, 256);
                    return auxDirEntry;
                }
            }
            else
            {
                short_name = get_short_name(auxDirEntry);
                if (strcmp(short_name, arg) == 0)
                {
                    auxDirEntry->LongName = NULL;
                    return auxDirEntry;
                }
            }
            has_long_name = 0;
        }
    }
}

dir_entry *get_entry_from_dot_entry(int fd, dir_entry *entry, dir_entry *start)
{
    dir_entry *auxDirEntry = malloc(sizeof(dir_entry));
    uint8_t buffer[DIR_ENTRY_SIZE];

    long_dir_entry *longDirEntry = malloc(sizeof(long_dir_entry));
    int8_t seq_number;
    uint16_t long_filename[256];
    int multp = 0;
    int has_long_name = 0;

    lseek(fd, first_sec(first_clu(start)) * BS->BytesPerSec, SEEK_SET);
    while (1)
    {
        if (read(fd, &buffer, DIR_ENTRY_SIZE) < 1)
            perror("read dir entry failure");

        if (buffer[0] == 0x00)
        {
            printf("no found in %s\n", start->Name);
            free(auxDirEntry);
            return NULL;
        }

        if (is_long_file(buffer[0x0B]))
        {
            has_long_name = 1;
            memcpy(longDirEntry, buffer, sizeof(long_dir_entry));

            if ((longDirEntry->LDIR_Ord & 0x40) == 0x40)
                seq_number = longDirEntry->LDIR_Ord - 0x40;
            else
                seq_number = longDirEntry->LDIR_Ord;

            multp = copy_name_fields(longDirEntry, &long_filename[13 * (seq_number - 1)]);
            if ((longDirEntry->LDIR_Ord & 0x40) && multp)
                long_filename[13 * seq_number] = 0x0000;
        }
        else
        {
            memcpy(auxDirEntry, buffer, sizeof(buffer));
            if (first_clu(auxDirEntry) == first_clu(entry))
            {
                if (has_long_name){
                    auxDirEntry->LongName = malloc(sizeof(uint16_t) * 256);
                    memcpy(auxDirEntry->LongName, long_filename, 256);
                }
                else
                    auxDirEntry->LongName = NULL;
                return auxDirEntry;
            }       
            if (is_dir(auxDirEntry) && auxDirEntry->Name[0] != '.')
            {
                get_entry_from_dot_entry(fd, entry, auxDirEntry);
            }
            has_long_name = 0;
        }
    }
}

void cat(int fd, char *arg)
{
    dir_entry *auxDirEntry = get_entry(fd, arg);
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

    char *content = get_file_content(fd, first_clu(auxDirEntry), auxDirEntry->FileSize);
    printf("%s", content);
    free(content);
    free(auxDirEntry);
    return;
}

void cd(int fd, char *arg)
{
    dir_entry *auxDirEntry;

    if (strcmp(arg, ".") == 0)
        return;

    if (strcmp(arg, "..") == 0)
    {
        *currDirEntry = *fatherDirEntry;
        //before ok

        auxDirEntry = get_entry(fd, arg);
        //apaga o long name de currDirEntry e de fatherDirEntry

        if (!auxDirEntry || first_clu(auxDirEntry) == 0x00)
            *fatherDirEntry = *rootDirEntry;
        else
            *fatherDirEntry = *get_entry_from_dot_entry(fd, auxDirEntry, rootDirEntry);

        free(auxDirEntry);
    }
    else
    {
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
    }
    return;
}

void ls(int fd, char *arg)
{
    dir_entry *auxDirEntry = (dir_entry *)malloc(sizeof(dir_entry));

    uint8_t buffer[DIR_ENTRY_SIZE];

    long_dir_entry *longDirEntry = malloc(sizeof(long_dir_entry));
    int8_t seq_number;
    uint16_t long_filename[256];

    int multp = 0;
    int show_hidden = 0;
    int show_deleted = 0;
    int has_long_name = 0;
    if (arg && strcmp(arg, "-hidden") == 0)
        show_hidden = 1;

    if (arg && strcmp(arg, "-deleted") == 0)
        show_deleted = 1;

    lseek(fd, first_sec(first_clu(currDirEntry)) * BS->BytesPerSec, SEEK_SET);

    print_ls_header();

    while (1)
    {
        if (read(fd, buffer, DIR_ENTRY_SIZE) < 1)
            perror("read dir entry failure");

        if (buffer[0] == 0x00)
        {
            free(auxDirEntry);
            free(longDirEntry);
            return;
        }

        if (is_long_file(buffer[0x0B]))
        {
            if (buffer[0] != 0xE5)
            {
                has_long_name = 1;
                memcpy(longDirEntry, buffer, sizeof(long_dir_entry));

                if ((longDirEntry->LDIR_Ord & 0x40) == 0x40)
                    seq_number = longDirEntry->LDIR_Ord - 0x40;
                else
                    seq_number = longDirEntry->LDIR_Ord;

                multp = copy_name_fields(longDirEntry, &long_filename[13 * (seq_number - 1)]);
                if ((longDirEntry->LDIR_Ord & 0x40) && multp)
                    long_filename[13 * seq_number] = 0x0000;
            }
        }
        else
        {
            memcpy(auxDirEntry, buffer, sizeof(buffer));
            if (((auxDirEntry->Name[0] == '.') || (has_long_name && long_filename[0] == '.')) && !show_hidden)
                ;
            else if (is_deleted(auxDirEntry->Name[0]) && !show_deleted)
                ;
            else
                show_entry(auxDirEntry, long_filename, has_long_name);
            has_long_name = 0;
        }
    }
    free(auxDirEntry);
    free(longDirEntry);
}

void exec(int fd, char **tokens)
{
    char *cmd = tokens[0];
    char *args = tokens[1];

    for (size_t i = 0; i < 6; i++)
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