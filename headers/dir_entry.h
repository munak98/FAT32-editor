/*
This header file defines the directory entry
structure acording to the attributes and offsets given in the
Microsoft Extensible Firmware Initiative FAT32 File System Specification,
Version 1.03
 */

#ifndef DIR_H
#define DIR_H
#define DIR_ENTRY_SIZE 32
#include <inttypes.h>

#pragma pack(push)
#pragma pack(1)

struct dir_struct
{
    char Name[11];
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

typedef struct dir_struct dir_entry;

dir_entry *fatherDirEntry;
dir_entry *currDirEntry;

void init_DirEntry();
void show_entry(dir_entry *);

#endif