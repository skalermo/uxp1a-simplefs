#ifndef BLOCK_LINKS_H
#define BLOCK_LINKS_H

#include <stdint.h>
#include <stddef.h> // offsetof function to use with structs.
#include <limits.h>

#include "fs_defines.h"

///////////////////////////////////
//  Defines
//////////////////////////////////


#define FS_NUMBER_OF_BLOCKS 65536
#define FS_LOCK_SIZE 1024 // in bytes
#define FS_EMPTY_BLOCK_VALUE UINT32_MAX


///////////////////////////////////
//  Structs
//////////////////////////////////

struct BlockLinks{
    uint32_t block_num[FS_NUMBER_OF_BLOCKS];
};

struct BlockStat{
    uint8_t block_bitmap[(FS_NUMBER_OF_BLOCKS / 8) + 1];
    uint32_t used_data_blocks;
};



///////////////////////////////////
//  Struct functions
//////////////////////////////////


int fs_get_data(uint32_t from, uint32_t to, uint32_t initialBlockNumber, void* receivedData);

int fs_save_data(uint32_t from, uint32_t to, uint32_t initialBlockNumber, void* dataToRecord);


uint32_t fs_get_next_block_number(uint32_t blockNumber);

int fs_allocate_new_block(uint32_t blockNumerInChain);




///////////////////////////////////
//  Other functions
//////////////////////////////////

int fs_create_blocks_stuctures_in_shm(uint32_t offsetLinks, uint32_t offsetBitmap);

int fs_lock_blocks();

int fs_unlock_blocks();




#endif