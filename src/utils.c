#include <stdio.h>
#include "../headers/utils.h"
#include "../headers/dir_entry.h"
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <regex.h>
#include <string.h>

void show_prompt()
{
    if (currDirEntry->LongName)
        print_long_name(currDirEntry->LongName);
    else
        printf("%s", currDirEntry->Name);
    printf(" -> ");
}

char *read_line(void)
{
    int line_size = 11;                            // initial buffer size
    int i = 0;                                     // index to the line array
    char *line = malloc(sizeof(char) * line_size); // array to store the command
    int c;                                         // variable to get the enter caracters

    if (!line)
    {
        fprintf(stderr, "allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        c = getchar();

        if (c == EOF || c == '\n')
        {
            line[i] = '\0';
            break;
        }
        else
        {
            line[i] = c;
        }
        i++;
    }
    return line;
}

char *ltrim(char *s, char to_trim)
{
    while (*s == to_trim)
        s++;
    return s;
}

char *rtrim(char *s, char to_trim)
{
    char *back = s + strlen(s);
    while (*--back == to_trim)
        ;
    *(back + 1) = '\0';
    return s;
}

char *trim(char *s, char to_trim)
{
    return rtrim(ltrim(s, to_trim), to_trim);
}

char **split(char *line, char *delimiter)
{
    int size = 11;
    char **tokens = malloc(size * sizeof(char *));
    char *token;
    if (!tokens)
    {
        fprintf(stderr, "allocation error\n");
        exit(EXIT_FAILURE);
    }

    tokens[0] = trim(strtok(line, delimiter), ' ');
    if ((token = strtok(NULL, "\n")) != NULL)
        tokens[1] = token;
    else
        tokens[1] = NULL;

    return tokens;
}

void free_content(uint8_t **vector)
{
    int i = 0;
    while (vector[i] != NULL)
    {
        free(vector[i]);
        i++;
    }
}

void print_ls_header()
{
    printf("\t%-12s%-10s\t\n", "Type", "Name");
    printf("\t===============================\n");
}

int is_equal(char *name, uint16_t *long_name)
{
    size_t i = 0;
    for (i = 0; i < strlen(name); i++)
    {
        if (name[i] != long_name[i])
        {
            return 0;
        }
    }
    return 1;
}