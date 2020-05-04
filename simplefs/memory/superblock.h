#ifndef SIMPLEFS_SUPERBLOCK_H
#define SIMPLEFS_SUPERBLOCK_H

#include <stdint.h>
#include <stddef.h> // offsetof function to use with structs.
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "inode.h"
#include "block_links.h"


///////////////////////////////////
//  Defines
//////////////////////////////////

#define FS_SHM_NAME "Simple_fs_shm"

//
// TODO
//
#define FS_ENTIRE_SIZE 0 // in bytes


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

    uint32_t inode_table_pointer;
    uint32_t inode_bitmap_pointer;
};


///////////////////////////////////
//  Struct functions
//////////////////////////////////


/**
 * @brief Get data from superblock with datatype of uint32_t.
 * 
 * @param index - index of the variable form superblock.
 * @param addr - address of the mapped shared memory.
 * @return uint32_t - returned value.
 */
uint32_t fs_get_from_superblock_uint32(uint8_t index, void* addr);

/**
 * @brief Get data from superblock with datatype of uint16_t.
 * 
 * @param index - index of the variable form superblock.
 * @param addr - address of the mapped shared memory.
 * @return uint16_t returned value.
 */
uint16_t fs_get_data_from_superblock_uint16(uint8_t index, void* addr);



/**
 * @brief Save data to superblock with datatype of uint16_t.
 * 
 * @param index - index of the variable form superblock.
 * @param data - the data to be saved in superblock. It is a uint16_t type.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * Otherwise error code.
 */
int8_t fs_save_data_to_superblock_uint16(uint8_t index, uint16_t data, void* addr);

/**
 * @brief Save data to superblock with datatype of uint32_t.
 * 
 * @param index - index of the variable form superblock.
 * @param data - the data to be saved in superblock. It is a uint32_t type.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * Otherwise error code.
 */
int8_t fs_save_data_to_superblock_uint32(uint8_t index, uint32_t data, void* addr);

/**
 * @brief Get copy of the superblock.
 * @details The copy of the superblock is not protected in any kind.
 * It is simply added to improve performance. 
 * 
 * @param superblockCopy - pointer where the copy will be stored
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * Otherwise error code.
 */
int8_t fs_get_superblock_copy(struct Superblock* superblockCopy, void* addr);


///////////////////////////////////
//  Pointer getters
//////////////////////////////////

void* fs_get_open_file_table_ptr(void* addr);

void* fs_get_open_file_bitmap_ptr(void* addr);


void* fs_get_block_links_ptr(void* addr);

void* fs_get_block_bitmap_ptr(void* addr);


void* fs_get_inode_table_ptr(void* addr);

void* fs_get_inode_bitmap_ptr(void* addr);


///////////////////////////////////
//  Other functions
//////////////////////////////////

/**
 * @brief Creates initial superblock structure in shared memory.
 * 
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * Otherwise error code.
 */
int8_t fs_create_superblock_in_shm(void* addr); // TO_CHECK



#endif