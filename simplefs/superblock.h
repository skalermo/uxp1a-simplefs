#ifndef SUPERBLOCK_H
#define SUPERBLOCK_H

#include <stdint.h>
#include <stddef.h> // offsetof function to use with structs.

#include "fs_defines.h"


///////////////////////////////////
//  Structs
//////////////////////////////////



struct Superblock {
    uint16_t max_number_of_files;
    uint16_t filesystem_checks;
    uint16_t data_block_size;
    uint32_t fs_size;
    uint32_t open_file_table_pointer;
    uint32_t open_file_bitmap_pointer;
    uint32_t block_links_pointer;
    uint32_t block_bitmap_pointer;
    uint32_t inode_bitmap_pointer;
    uint32_t inode_table_pointer;
};


///////////////////////////////////
//  Struct functions
//////////////////////////////////



uint32_t fs_get_from_superblock_uint32(uint8_t index);

uint16_t fs_get_data_from_superblock_uint16(uint8_t index);


int fs_save_data_to_superblock_uint16(uint8_t index, uint16_t data);

int fs_save_data_to_superblock_uint32(uint8_t index, uint32_t data);


int fs_get_superblock_copy(Superblock* superblockCopy);



///////////////////////////////////
//  Other functions
//////////////////////////////////

int fs_create_superblock_in_shm();

int fs_lock_superblock();

int fs_unlock_superblock();


#endif