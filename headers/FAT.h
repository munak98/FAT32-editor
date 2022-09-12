#include <inttypes.h>

#ifndef FAT_H
#define FAT_H

#define DIR_ENTRY_LAST_AND_UNUSED 0x0
#define DIR_ENTRY_UNUSED 0xE5

int first_sec(int);

void cd(int, char *);
void ls(int, char *);
void cat(int, char *);
void undelete(int, char *);
void delete (int, char *);
void hide(int, char *);
void unhide(int, char *);
void exit_f(int, char *);

void exec(int, char **);

#endif