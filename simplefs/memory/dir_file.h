#ifndef DIR_FILE_H
#define DIR_FILE_H

#include <stdint.h>
#include <stddef.h> // offsetof
#include "inode.h"
#include "block_links.h"

///////////////////////////////////
//  Defines
//////////////////////////////////

#define FS_NAME_SIZE 61

///////////////////////////////////
//  Structs
//////////////////////////////////

struct DirEntry{
    uint8_t name[FS_NAME_SIZE];
    uint8_t name_len;
    uint16_t inode_number;
};

struct DirFile{
    struct DirEntry* entry; // we dont know how many blocks will be allocated.
};


///////////////////////////////////
//  Struct functions
//////////////////////////////////

/**
 * @brief Save a name to the given directory entry index.
 * 
 * @param blockNumber - index of a block where the folder's file begins.
 * @param dirEntryIndex - index within a file (blockchain) where you want to save a name.
 * @param name - a name to be saved.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * Otherwise error code.
 */
int8_t fs_save_data_to_dir_entry_name(uint32_t blockNumber, uint32_t dirEntryIndex, char* name, void* addr);

/**
 * @brief Save an inode index to the given directory entry index.
 * 
 * @param blockNumber - index of a block where the folder's file begins.
 * @param dirEntryIndex - index within a file (blockchain) where you want to save an inode index.
 * @param inodeNumber - the inode index that will be saved.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * Otherwise error code.
 */
int8_t fs_save_data_to_dir_entry_inode_number(uint32_t blockNumber, uint32_t dirEntryIndex, uint16_t inodeNumber, void* addr);


/**
 * @brief Get a name of the given directory entry index.
 * 
 * @param blockNumber - index of a block where the folder's file begins.
 * @param dirEntryIndex - index within a file (blockchain) from where you want to get a name.
 * @param name - pointer where the name will be saved. It should have at least allocated FS_NAME_SIZE bytes.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * Otherwise error code.
 */
int8_t fs_get_data_from_dir_entry_name(uint32_t blockNumber, uint32_t dirEntryIndex, char* name, void* addr);

/**
 * @brief Get an inode index of the given directory entry index.
 * 
 * @param blockNumber - index of a block where the folder's file begins.
 * @param dirEntryIndex - index within a file (blockchain) from where you want to get an inode index.
 * @param inodeNumber - pointer to the inode where the inode index will be saved.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * Otherwise error code.
 */
int8_t fs_get_data_from_dir_entry_inode_number(uint32_t blockNumber, uint32_t dirEntryIndex, uint16_t* inodeNumber, void* addr);


/**
 * @brief Get an copy of the specified directory entry.
 * 
 * @param blockNumber - index of a block where the folder's file begins.
 * @param dirEntryIndex - index within a file (blockchain) from where you want to get this directory entry.
 * @param dirEntryCopy - pointer where the directory entry will be saved.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * Otherwise error code.
 */
int8_t fs_get_dir_entry_copy(uint32_t blockNumber, uint32_t dirEntryIndex, DirEntry* dirEntryCopy, void* addr);

/**
 * @brief Get an copy of an entire folder's file.
 * 
 * @param blockNumber - index of a block where the folder's file begins.
 * @param dirFileCopy - pointer where the direcotry file will be saved.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * Otherwise error code.
 */
int8_t fs_get_dir_file_copy(uint32_t blockNumber, struct DirFile* dirFileCopy, void* addr);


/**
 * @brief Create clean folder's file.
 * @details It consumes one block form data blocks.
 * 
 * @param blockNumber - pointer where the block index of a folder's file will be saved.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * Otherwise error code.
 */
int8_t fs_create_dir_file(uint32_t* blockNumber);

/**
 * @brief Create clean folder's file and save the provided directory entry at the beginning.
 * 
 * @param blockNumber - pointer where the allocated block index of a folder's file will be saved.
 * @param dirFileToSave - pointer to the directory entry structure that will be saved.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * Otherwise error code.
 */
int8_t fs_create_and_save_dir_file(uint32_t* blockNumber, struct DirEntry* dirEntryToSave, void* addr);


/**
 * @brief Get an index of a free directory entry in folder's file.
 * 
 * @param blockNumber - index of a block where the folder's file begins.
 * @param dirEntryIndex - pointer where the index of a free directory entry will be saved.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * Otherwise error code.
 */
int8_t fs_get_free_dir_entry(uint32_t blockNumber, uint32_t* dirEntryIndex, void* addr);

/**
 * @brief Get an index of a free directory entry in folder's file and save the provided directory entry.
 * 
 * @param blockNumber - index of a block where the folder's file begins.
 * @param dirEntryIndex - pointer where the index of a free directory entry will be saved.
 * @param dirEntryToSave - pointer to the directory entry structure that will be saved.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * Otherwise error code.
 */
int8_t fs_occupy_free_dir_entry(uint32_t blockNumber, uint32_t* dirEntryIndex, struct DirEntry* dirEntryToSave, void* addr);


#endif