#include <inttypes.h>

char *read_line();
char **split(char *, char *);
char *rtrim(char *, char);
char *ltrim(char *, char);
void show_prompt();

void init_buffer(uint8_t *);

void free_content(uint8_t **);
void print_ls_header();
int is_equal(char *, uint16_t *);
