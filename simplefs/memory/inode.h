/*
 * inode.h
 *
 *      Author: Kordowski Mateusz
 */

#ifndef SIMPLEFS_INODE_H
#define SIMPLEFS_INODE_H

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h> // offsetof function to use with structs.

#include "utils.h"


// Inode mode defines

#define O_RDONLY    0x01
#define O_WRONLY    0x02
#define O_RDWR      0x03

// O_RDONLY | O_WRONLY == O_RDWR

#define IS_DIR      0x10
#define IS_FILE     0x30

// IS_DIR | IS_FILE == IS_FILE


///////////////////////////////////
//  Structs
//////////////////////////////////

struct Inode {
    uint32_t block_index;
    uint16_t file_size;
    uint16_t readers;
    uint16_t writers;
    uint8_t mode;
    uint8_t ref_count;
    uint8_t padding[2];
};


struct InodeTable {
    struct Inode* inode; //[FS_MAX_NUMBER_OF_INODES];
};

struct InodeStat {
    uint8_t* inode_bitmap; //[FS_MAX_NUMBER_OF_INODES_BY_8];
    uint16_t inode_used;
};


///////////////////////////////////
//  Struct functions
//////////////////////////////////

/**
 * @brief Save some data with a type uint8_t to the inode.
 * 
 * @param inodeIndex - index of an inode in inode table.
 * @param index - index in inode structure.
 * @param data - data to save.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * -1 if the index points to the non-existent variable in structue.
 * -2 if the index in relation to data type was wrong.
 */
int8_t fs_save_data_to_inode_uint8(uint16_t inodeIndex, uint8_t index, uint8_t data, void* addr);

/**
 * @brief Save some data with a type uint16_t to the inode.
 * 
 * @param inodeIndex - index of an inode in inode table.
 * @param index - index in inode structure.
 * @param data - data to save.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * -1 if the index points to the non-existent variable in structue.
 * -2 if the index in relation to data type was wrong.
 */
int8_t fs_save_data_to_inode_uint16(uint16_t inodeIndex, uint8_t index, uint16_t data, void* addr);

/**
 * @brief Save some data with a type uint32_t to the inode.
 * 
 * @param inodeIndex - index of an inode in inode table.
 * @param index - index in inode structure.
 * @param data - data to save.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * -1 if the index points to the non-existent variable in structue.
 * -2 if the index in relation to data type was wrong.
 */
int8_t fs_save_data_to_inode_uint32(uint16_t inodeIndex, uint8_t index, uint32_t data, void* addr);


/**
 * @brief Get data of type uint16_t from inode with specified index.
 * 
 * @param inodeIndex - index of an inode in inode table.
 * @param index - index in inode structure.
 * @param data - pointer where data will be saved.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * -1 if the index points to the non-existent variable in structue.
 * -2 if the index in relation to data type was wrong.
 */
int8_t fs_get_data_from_inode_uint8(uint16_t inodeIndex, uint8_t index, uint8_t* data, void* addr);


/**
 * @brief Get data of type uint16_t from inode with specified index.
 * 
 * @param inodeIndex - index of an inode in inode table.
 * @param index - index in inode structure.
 * @param data - pointer where data will be saved.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * -1 if the index points to the non-existent variable in structue.
 * -2 if the index in relation to data type was wrong.
 */
int8_t fs_get_data_from_inode_uint16(uint16_t inodeIndex, uint16_t index, uint16_t* data, void* addr);

/**
 * @brief Get data of type uint32_t from inode with specified index.
 * 
 * @param inodeIndex - index of an inode in inode table.
 * @param index - index in inode structure.
 * @param data - pointer where data will be saved.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * -1 if the index points to the non-existent variable in structue.
 * -2 if the index in relation to data type was wrong.
 */
int8_t fs_get_data_from_inode_uint32(uint16_t inodeIndex, uint32_t index, uint32_t* data, void* addr);


/**
 * @brief Get copy of the inode specified by index.
 * 
 * @param inodeIndex - index of an inode in inode table.
 * @param inodeCopy - pointer where inode will be saved.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * No other errors.
 */
int8_t fs_get_inode_copy(uint32_t inodeIndex, struct Inode* inodeCopy, void* addr);


/**
 * @brief Get an free inode index.
 * There is no rule which free inode will be chosen.
 * It does not mark this inode in bitmap.
 * 
 * @param inodeIndex - pointer where the free inode index will be saved.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful or
 * -1 if there was no more free inode available.
 */
int8_t fs_get_free_inode(uint16_t* inodeIndex, void* addr);

/**
 * @brief Get an free inode index and save the inode structure.
 * There is no rule which free inode will be chosen.
 * It does mark this inode in bitmap as used and change the inode counter.
 * 
 * @param inodeIndex - pointer where the free inode index will be saved.
 * @param inodeToSave - inode that will be saved in inode table.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful or
 * -1 if there was no more free inode available
 * In case of negative values the inodeIndex will be intact.
 */
int8_t fs_occupy_free_inode(uint16_t* inodeIndex, struct Inode* inodeToSave, void* addr);

/**
 * @brief Mark inode as used in inode bitmap.
 * It does not change the number of currently used inodes.
 * 
 * @param inodeIndex - index of an inode.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * No other errors.
 */
int8_t fs_mark_inode_as_used(uint16_t inodeIndex, void* addr);

/**
 * @brief Mark inode as free in inode bitmap.
 * It does not change the number of currently used inodes.
 * 
 * @param inodeIndex - index of an inode.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * No other errors.
 */
int8_t fs_mark_inode_as_free(uint16_t inodeIndex,void* addr);

/**
 * @brief Get the number of currently used inodes.
 * 
 * @param addr - address of the mapped shared memory.
 * @return uint32_t - the number of used inodes.
 * No other errors.
 */
uint32_t fs_get_used_inodes(void* addr);

/**
 * @brief Set the number of currently used inodes.
 * 
 * @param saveUsedInodes - value that will be saved in stat structure.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was succsessful.
 * No other errors.
 */
int8_t fs_set_used_inodes(uint32_t saveUsedInodes, void* addr);

///////////////////////////////////
//  Other functions
//////////////////////////////////

/**
 * @brief Creates an initial inode structures in shared memory.
 * Superblock must be already created.
 * 
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * No other errors.
 */
int8_t fs_create_inode_structures_in_shm(void* addr); // TO_CHECK, TODO


///////////////////////////////////
//  Private functions
//////////////////////////////////

uint8_t inner_fs_get_InodeStat_used_sizeof();

///////////////////////////////////
//  Forward declaration
//////////////////////////////////

void* fs_get_open_file_table_ptr(void* addr);

void* fs_get_open_file_bitmap_ptr(void* addr);


void* fs_get_inode_table_ptr(void* addr);

void* fs_get_inode_bitmap_ptr(void* addr);


uint16_t fs_get_data_block_size(void* addr);

uint16_t fs_get_max_number_of_inodes(void* addr);

uint16_t fs_get_max_number_of_open_files(void* addr);


#endif