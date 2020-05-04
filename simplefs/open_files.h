#ifndef OPEN_FILES_H
#define OPEN_FILES_H

#include <stdint.h>
#include <stddef.h> // offsetof
#include "inode.h"


///////////////////////////////////
//  Structs
//////////////////////////////////


struct OpenFile{
    uint16_t mode;
    uint16_t inode_num;
    uint32_t offset;
};


struct OpenFileTable {
    struct OpenFile file[FS_MAX_NUMBER_OF_INODES];
};

struct OpenFileStat{
    uint8_t open_file_bitmap[(FS_MAX_NUMBER_OF_INODES / 8) + 1];
    uint16_t opened_files;
};



///////////////////////////////////
//  Struct functions
//////////////////////////////////



int fs_save_data_to_open_file_uint16(uint8_t openFileIndex, uint16_t data);

int fs_save_data_to_open_file_uint32(uint8_t openFileIndex, uint32_t data);


uint16_t fs_get_data_from_open_file_uint16(uint8_t openFileIndex);

uint32_t fs_get_data_from_open_file_uint32(uint8_t openFileIndex);


int fs_get_open_file_copy(uint32_t openFileIndex, OpenFile* openFileCopy);


int fs_get_free_open_file(uint32_t* openFileIndex);




///////////////////////////////////
//  Other functions
//////////////////////////////////



int fs_create_open_file_table_stuctures_in_shm(uint32_t offsetOpenFileTable, uint32_t offsetBitmap);

int lockReadOpenFile();

int lockWriteOpenFile();


#endif