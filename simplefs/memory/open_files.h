#ifndef OPEN_FILES_H
#define OPEN_FILES_H

#include <stdint.h>
#include <stddef.h> // offsetof
#include "inode.h"

///////////////////////////////////
//  Defines
//////////////////////////////////

#define FS_MAX_NUMBER_OF_OPEN_FILES 1024


///////////////////////////////////
//  Structs
//////////////////////////////////


struct OpenFile{
    uint16_t mode;
    uint16_t inode_num;
    uint32_t offset;
};


struct OpenFileTable {
    struct OpenFile file[FS_MAX_NUMBER_OF_OPEN_FILES];
};

struct OpenFileStat{
    uint8_t open_file_bitmap[(FS_MAX_NUMBER_OF_OPEN_FILES / 8) + 1];
    uint16_t opened_files;
};



///////////////////////////////////
//  Struct functions
//////////////////////////////////


/**
 * @brief Save some data whith a type uint16_t to the opened file.
 * 
 * @param openFileIndex - index of an open file structure in open file table.
 * @param index - index within open file structure.
 * @param data - data to save.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * Otherwise error code.
 */
int8_t fs_save_data_to_open_file_uint16(uint16_t openFileIndex, uint8_t index, uint16_t data, void* addr);

/**
 * @brief Save some data whith a type uint32_t to the opened file.
 * 
 * @param openFileIndex - index of an open file structure in open file table.
 * @param index - index within open file structure.
 * @param data - data to save.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * Otherwise error code.
 */
int8_t fs_save_data_to_open_file_uint32(uint16_t openFileIndex, uint8_t index, uint32_t data, void* addr);


/**
 * @brief Get data of type uint16_t from open file structure with specified index.
 * 
 * @param openFileIndex - index of an open file structure in open file table.
 * @param index - index within open file structure.
 * @param data - pointer where the read data will be saved.
 * @param addr - address of the mapped shared memory.
 * @return uint8_t - - 0 if operation was successful.
 * Otherwise error code.
 */
uint8_t fs_get_data_from_open_file_uint16(uint16_t openFileIndex, uint8_t index, uint16_t* data, void* addr);

/**
 * @brief Get data of type uint32_t from open file structure with specified index.
 * 
 * @param openFileIndex - index of an open file structure in open file table.
 * @param index - index within open file structure.
 * @param data - pointer where the read data will be saved.
 * @param addr - address of the mapped shared memory.
 * @return uint8_t - 0 if operation was successful.
 * Otherwise error code.
 */
uint8_t fs_get_data_from_open_file_uint32(uint16_t openFileIndex, uint8_t index, uint32_t* data, void* addr);


/**
 * @brief Get copy of the open file structure specified by index.
 * 
 * @param openFileIndex - index of an open file structure in open file table.
 * @param openFileCopy - pointer where the read open file will be saved.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * Otherwise error code.
 */
int8_t fs_get_open_file_copy(uint16_t openFileIndex, struct OpenFile* openFileCopy, void* addr);


/**
 * @brief Get an free open file index.
 * There is no rule which free open file index will be chosen.
 * 
 * @param openFileIndex - pointer where the open file index will be saved.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * Otherwise error code.
 */
int8_t fs_get_free_open_file(uint16_t* openFileIndex, void* addr);

/**
 * @brief 
 * 
 * @param openFileIndex - pointer where the open file index will be saved.
 * @param openFileToSave - - pointer to the open file structure that will be saved.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * Otherwise error code.
 */
int8_t fs_occupy_free_open_file(uint32_t* openFileIndex, struct OpenFile* openFileToSave, void* addr);


///////////////////////////////////
//  Other functions
//////////////////////////////////


/**
 * @brief Creates a initial open file table with other structres in shared memory.
 * 
 * @param offsetTable - offset of an open file table counted from where the superblock is.
 * @param offsetBitmap - offset of an open file bitmap structure counted from where the superblock is.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * Otherwise error code.
 */
int8_t fs_create_open_file_table_stuctures_in_shm(uint32_t offsetOpenFileTable, uint32_t offsetBitmap, void* addr);




#endif