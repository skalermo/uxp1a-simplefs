/*
 * open_files.h
 *
 *      Author: Kordowski Mateusz
 */

#ifndef SIMPLEFS_OPEN_FILES_H
#define SIMPLEFS_OPEN_FILES_H

#include <stdint.h>
#include <stddef.h> // offsetof
#include "inode.h"

///////////////////////////////////
//  Defines
//////////////////////////////////

//#define FS_MAX_NUMBER_OF_OPEN_FILES 1024


///////////////////////////////////
//  Structs
//////////////////////////////////


struct OpenFile{
    uint16_t mode;
    uint16_t inode_num;
    uint32_t offset;
};


struct OpenFileTable {
    struct OpenFile* file; //[FS_MAX_NUMBER_OF_OPEN_FILES];
};

struct OpenFileStat{
    uint8_t* open_file_bitmap; //[(FS_MAX_NUMBER_OF_OPEN_FILES / 8) + 1];
    uint16_t opened_files;
};



///////////////////////////////////
//  Struct functions
//////////////////////////////////


/**
 * @brief Save some data with a type uint16_t to the opened file.
 * 
 * @param openFileIndex - index of an open file structure in open file table.
 * @param index - index within open file structure.
 * @param data - data to save.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * -1 if the index points to the non-existent variable in structue.
 * -2 if the index in relation to data type was wrong.
 */
int8_t fs_save_data_to_open_file_uint16(uint16_t openFileIndex, uint8_t index, uint16_t data, void* addr);

/**
 * @brief Save some data with a type uint32_t to the opened file.
 * 
 * @param openFileIndex - index of an open file structure in open file table.
 * @param index - index within open file structure.
 * @param data - data to save.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * -1 if the index points to the non-existent variable in structue.
 * -2 if the index in relation to data type was wrong.
 */
int8_t fs_save_data_to_open_file_uint32(uint16_t openFileIndex, uint8_t index, uint32_t data, void* addr);


/**
 * @brief Get data of type uint16_t from open file structure with specified index.
 * 
 * @param openFileIndex - index of an open file structure in open file table.
 * @param index - index within open file structure.
 * @param data - pointer where the read data will be saved.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * -1 if the index points to the non-existent variable in structue.
 * -2 if the index in relation to data type was wrong.
 */
int8_t fs_get_data_from_open_file_uint16(uint16_t openFileIndex, uint8_t index, uint16_t* data, void* addr);

/**
 * @brief Get data of type uint32_t from open file structure with specified index.
 * 
 * @param openFileIndex - index of an open file structure in open file table.
 * @param index - index within open file structure.
 * @param data - pointer where the read data will be saved.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * -1 if the index points to the non-existent variable in structue.
 * -2 if the index in relation to data type was wrong.
 */
int8_t fs_get_data_from_open_file_uint32(uint16_t openFileIndex, uint8_t index, uint32_t* data, void* addr);


/**
 * @brief Get copy of the open file structure specified by index.
 * 
 * @param openFileIndex - index of an open file structure in open file table.
 * @param openFileCopy - pointer where the open file will be saved.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * No other errors.
 */
int8_t fs_get_open_file_copy(uint16_t openFileIndex, struct OpenFile* openFileCopy, void* addr);


/**
 * @brief Get an free open file index.
 * There is no rule which free open file index will be chosen.
 * It does not mark this open file in bitmap.
 * 
 * @param openFileIndex - pointer where the open file index will be saved.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * -1 if some other error happend.
 * -2 if there is no more free open files structures.
 */
int8_t fs_get_free_open_file(uint16_t* openFileIndex, void* addr);

/**
 * @brief Get an free open file index and save the provided open file here,
 * There is no rule which free open file index will be chosen.
 * It does mark this open file in bitmap (set as used).
 * 
 * @param openFileIndex - pointer where the open file index will be saved.
 * @param openFileToSave - - pointer to the open file structure that will be saved.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * -1 if some other error happend.
 * -2 if there is no more free open files structures.
 */
int8_t fs_occupy_free_open_file(uint32_t* openFileIndex, struct OpenFile* openFileToSave, void* addr);

/**
 * @brief Mark open file as used in open file bitmap.
 * 
 * @param inodeIndex - index of an open file.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * No other errors.
 */
int8_t fs_mark_open_file_as_used(uint32_t openFileIndex, void* addr);

/**
 * @brief Mark open file as free in open file bitmap.
 * 
 * @param inodeIndex - index of an open file.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * No other errors.
 */
int8_t fs_mark_inode_as_free(uint32_t openFileIndex, void* addr);


///////////////////////////////////
//  Other functions
//////////////////////////////////


/**
 * @brief Creates a initial open file table with other structres in shared memory.
 * Superblock must be already created.
 * 
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * No other errors.
 */
int8_t fs_create_open_file_table_stuctures_in_shm(void* addr);  // TO_CHECK, TODO




#endif