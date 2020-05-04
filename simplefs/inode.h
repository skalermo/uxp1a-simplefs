#ifndef INODE_H
#define INODE_H

#include <stdint.h>
#include <stddef.h> // offsetof function to use with structs.

#include "fs_defines.h"



///////////////////////////////////
//  Defines
//////////////////////////////////


#define FS_MAX_NUMBER_OF_INODES 1024


///////////////////////////////////
//  Structs
//////////////////////////////////



struct Inode {
    uint32_t block_index;
    uint16_t file_size;
    uint8_t mode;
    uint8_t ref_count;
    uint8_t readers;
    uint8_t padding[3];
};


struct InodeTable {
    struct Inode inode[FS_MAX_NUMBER_OF_INODES];
};

struct InodeStat {
    uint8_t inode_bitmap[(FS_MAX_NUMBER_OF_INODES / 8) + 1];
    uint16_t inode_used;
};


///////////////////////////////////
//  Struct functions
//////////////////////////////////


int fs_save_data_to_inode_uint8(uint8_t inodeIndex, uint8_t data);

int fs_save_data_to_inode_uint16(uint8_t inodeIndex, uint16_t data);

int fs_save_data_to_inode_uint32(uint8_t inodeIndex, uint32_t data);


uint8_t fs_get_data_from_inode_uint8(uint8_t inodeIndex);

uint16_t fs_get_data_from_inode_uint16(uint8_t inodeIndex);

uint32_t fs_get_data_from_inode_uint32(uint8_t inodeIndex);


int fs_get_inode_copy(uint32_t inodeIndex, Inode* inodeCopy);


int fs_get_free_inode(uint32_t* inodeIndex);



///////////////////////////////////
//  Other functions
//////////////////////////////////

int fs_create_inode_structures_in_shm(uint32_t offsetTable, uint32_t offsetBitmap);

int fs_lock_inode();

int fs_unlock_inode();


#endif