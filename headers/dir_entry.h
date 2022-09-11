/*
This header file defines the directory entry
structure acording to the attributes and offsets given in the
Microsoft Extensible Firmware Initiative FAT32 File System Specification,
Version 1.03
 */

#ifndef DIR_H
#define DIR_H
#define DIR_ENTRY_SIZE 32
#define SIZE_LONG_NAME 10
#include <inttypes.h>
#include <string.h>
#include <wchar.h>
#include <uchar.h>

#pragma pack(push)
#pragma pack(1)

struct dir_struct
{
    uint8_t Name[11];
    uint8_t Attr;
    uint8_t NTRes;
    uint8_t CrtTimeTenth;
    uint16_t CrtTime;
    uint16_t CrtDate;
    uint16_t LstAccDate;
    uint16_t FstClusHI;
    uint16_t WrtTime;
    uint16_t WrtDate;
    uint16_t FstClusLO;
    uint32_t FileSize;
    uint16_t *LongName;
};

struct long_dir_struct
{
    uint8_t LDIR_Ord;
    uint16_t LDIR_Name1[5];
    uint8_t LDIR_Attr;
    uint8_t LDIR_Type;
    uint8_t LDIR_Chksum;
    uint16_t LDIR_Name2[6];
    uint16_t LDIR_FstClusLO;
    uint16_t LDIR_Name3[2];
};
#pragma pack(pop)

enum Attributes
{
    AttrReadOnly = 0x01,
    AttrHidden = 0x02,
    AttrSystem = 0x04,
    AttrVolumeLabel = 0x08,
    AttrDirectory = 0x10,
    AttrArchive = 0x20,
};

typedef struct long_dir_struct long_dir_entry;
typedef struct dir_struct dir_entry;

dir_entry *fatherDirEntry;
dir_entry *currDirEntry;
dir_entry *rootDirEntry;

int is_long_file(uint8_t);
int is_hidden(uint16_t);

int is_system(uint8_t);
int is_deleted(uint8_t);

int is_dir(dir_entry *);
int is_volum(dir_entry *);

uint16_t first_clu(dir_entry *);

char *get_short_name(dir_entry *);
uint8_t *get_long_name(long_dir_entry *);
void print_long_name(uint16_t *);
void init_DirEntry();
int copy_name_fields(long_dir_entry *, uint16_t *);
int size(uint8_t *, int);

void show_entry(dir_entry *, uint16_t *, int);

#endif