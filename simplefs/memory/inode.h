#ifndef INODE_H
#define INODE_H

#include <stdint.h>
#include <stddef.h> // offsetof function to use with structs.

#include "fs_defines.h"



///////////////////////////////////
//  Defines
//////////////////////////////////


#define FS_MAX_NUMBER_OF_INODES UINT16_MAX


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

/**
 * @brief Save some data whith a type uint8_t to the inode.
 * 
 * @param inodeIndex - index of an inode in inode table.
 * @param index - index in inode structure.
 * @param data - data to save.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * Otherwise error code.
 */
int8_t fs_save_data_to_inode_uint8(uint16_t inodeIndex, uint8_t index, uint8_t data, void* addr);

/**
 * @brief Save some data whith a type uint16_t to the inode.
 * 
 * @param inodeIndex - index of an inode in inode table.
 * @param index - index in inode structure.
 * @param data - data to save.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * Otherwise error code.
 */
int8_t fs_save_data_to_inode_uint16(uint16_t inodeIndex, uint8_t index, uint16_t data, void* addr);

/**
 * @brief Save some data whith a type uint32_t to the inode.
 * 
 * @param inodeIndex - index of an inode in inode table.
 * @param index - index in inode structure.
 * @param data - data to save.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * Otherwise error code.
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
 * Otherwise error code.
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
 * Otherwise error code.
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
 * Otherwise error code.
 */
int8_t fs_get_data_from_inode_uint32(uint16_t inodeIndex, uint32_t index, uint32_t* data, void* addr);


/**
 * @brief Get copy of the inode specified by index.
 * 
 * @param inodeIndex - index of an inode in inode table.
 * @param inodeCopy - pointer where inode will be saved.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * Otherwise error code.
 */
int8_t fs_get_inode_copy(uint32_t inodeIndex, struct Inode* inodeCopy, void* addr);


/**
 * @brief Get an free inode index.
 * There is no rule which free inode will be chosen.
 * 
 * @param inodeIndex - pointer where the free inode index will be saved.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * Otherwise error code.
 */
int8_t fs_get_free_inode(uint32_t* inodeIndex, void* addr);

/**
 * @brief Get an free inode index and save the inode structure.
 * 
 * @param inodeIndex - pointer where the free inode index will be saved.
 * @param inodeToSave - inode that will be saved in inode table.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * Otherwise error code.
 */
int8_t fs_occupy_free_inode(uint32_t* inodeIndex, struct Inode* inodeToSave, void* addr);



///////////////////////////////////
//  Other functions
//////////////////////////////////

/**
 * @brief Creates a initial inode structures in shared memory.
 * 
 * @param offsetTable - offset of a inode table counted from where the superblock is.
 * @param offsetBitmap - offset of a inode bitmap structure counted from where the superblock is.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * Otherwise error code.
 */
int8_t fs_create_inode_structures_in_shm(uint32_t offsetTable, uint32_t offsetBitmap, void* addr);


#endif