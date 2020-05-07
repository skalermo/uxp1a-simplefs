/*
 * dir_file.h
 *
 *      Author: Kordowski Mateusz
 */

#ifndef SIMPLEFS_DIR_FILE_H
#define SIMPLEFS_DIR_FILE_H

#include <stdint.h>
#include <stddef.h> // offsetof
#include "inode.h"
#include "block_links.h"

///////////////////////////////////
//  Defines
//////////////////////////////////

#define FS_NAME_SIZE 61
#define FS_MAIN_DIRECTORY_NAME "/"
#define FS_MAIN_DIRECTORY_NAME_SIZE 2

///////////////////////////////////
//  Structs
//////////////////////////////////

struct DirEntry{
    uint8_t name[FS_NAME_SIZE];
    uint8_t name_len; // in bytes
    uint16_t inode_number;
};

struct DirFile{
    struct DirEntry* entry; // we dont know how many blocks will be allocated.
};

///////////////////////////////////
//  Support structures
//////////////////////////////////

struct FS_create_dir_data{
    uint16_t thisDirInode;
    char* thisDirName;
    uint8_t thisDirNameLen;

    uint16_t prevoiusDirInode;
    char* prevoiusDirInodeName;
    uint8_t prevoiusDirInodeLen;
};


///////////////////////////////////
//  Struct functions
//////////////////////////////////

/**
 * @brief Save a name to the given directory entry index.
 * @details The entry index must be within allocated blocks so it is immposible to allocate more
 * memory through this function.
 * 
 * @param blockNumber - index of a block where the folder's file begins.
 * @param dirEntryIndex - index within a file (blockchain) where you want to save a name.
 * @param name - pointer to the name to be saved.
 * @param nameSize - size of a name. It must be less than FS_NAME_SIZE.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * -1 if the namesize is too big or directory entry index points to the unallocated block.
 */
int8_t fs_save_data_to_dir_entry_name(uint32_t blockNumber, uint32_t dirEntryIndex, char* name, uint8_t nameSize, void* addr);

/**
 * @brief Save an inode index to the given directory entry index.
 * @details The entry index must be within allocated blocks so it is immposible to allocate more
 * memory through this function.
 * 
 * @param blockNumber - index of a block where the folder's file begins.
 * @param dirEntryIndex - index within a file (blockchain) where you want to save an inode index.
 * @param inodeNumber - the inode index that will be saved.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * -1 if the inode number is 0 or directory entry index points to the unallocated block.
 */
int8_t fs_save_data_to_dir_entry_inode_number(uint32_t blockNumber, uint32_t dirEntryIndex, uint16_t inodeNumber, void* addr);


/**
 * @brief Get a name of the given directory entry index.
 * 
 * @param blockNumber - index of a block where the folder's file begins.
 * @param dirEntryIndex - index within a file (blockchain) from where you want to get a name.
 * @param name - pointer where the name will be saved. It should have allocate at least FS_NAME_SIZE bytes.
 * @param nameSize - pointer where the name size will be saved.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * -1 if the directory entry index points to the unallocated block.
 */
int8_t fs_get_data_from_dir_entry_name(uint32_t blockNumber, uint32_t dirEntryIndex, char* name, uint8_t* nameSize, void* addr);

/**
 * @brief Get an inode index of the given directory entry index.
 * 
 * @param blockNumber - index of a block where the folder's file begins.
 * @param dirEntryIndex - index within a file (blockchain) from where you want to get an inode index.
 * @param inodeNumber - pointer to the inode where the inode index will be saved.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * -1 if the directory entry index points to the unallocated block.
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
 * -1 if the directory entry index points to the unallocated block.
 */
int8_t fs_get_dir_entry_copy(uint32_t blockNumber, uint32_t dirEntryIndex, struct DirEntry* dirEntryCopy, void* addr);

/**
 * @brief Get an copy of an entire folder's file.
 * @details It is assumed that the provided blockNumber is used as directory file.
 * 
 * @param blockNumber - index of a block where the folder's file begins.
 * @param dirFileCopy - pointer where the direcotry file will be saved. 
 * In DirFile there is pointer that is used to malloc a table of DirEntry.
 * Do not forget about freeing this memory later.
 * 
 * @param sizeofDirFileCopy - pointer where the sizeof DirFile structure will be saved.
 * 
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * No other errors.
 */
int8_t fs_get_dir_file_copy(uint32_t blockNumber, struct DirFile* dirFileCopy, uint32_t* sizeofDirFileCopy, void* addr);


/**
 * @brief Create clean folder's file with initial structures to '.' and '..'.
 * @details It consumes one block form data blocks.
 * 
 * @param blockNumber - pointer where the block index of a folder's file will be saved.
 * @param inodesData - pointer to the helping structure that have information about inodes used in creation of inode folder.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * -1 if the allocation of a block failed.
 */
int8_t fs_create_dir_file(uint32_t* blockNumber, struct FS_create_dir_data* inodesData, void* addr);

/**
 * @brief Create clean folder's file with initial structures to '.' , '..' and save the provided directory entry at the beginning.
 * @details It is assumed that in one block there can be at least 3 DirEntry structures.
 * 
 * @param blockNumber - pointer where the allocated block index of a folder's file will be saved.
 * @param inodesData - pointer to the helping structure that have information about inodes used in creation of inode folder.
 * @param dirFileToSave - pointer to the directory entry structure that will be saved.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * -1 if the allocation of a block failed.
 */
int8_t fs_create_and_save_dir_file(uint32_t* blockNumber, struct FS_create_dir_data* inodesData, struct DirEntry* dirEntryToSave, void* addr);

/**
 * @brief Creates a main directory.
 * It must be used only at initialization of the file system before any other folder or file is created.
 * The first two entries are the same and have information about main folder.
 * 
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * No other errors.
 */
int8_t fs_create_main_folder(void* addr);

/**
 * @brief Get an index of a free directory entry in folder's file.
 * It can allocate additional block of data if it is needed.
 * 
 * @param blockNumber - index of a block where the folder's file begins.
 * @param dirEntryIndex - pointer where the index of a free directory entry will be saved.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * -1 if it failed to allocate additional block of data. It does not found free directory entry.
 */
int8_t fs_get_free_dir_entry(uint32_t blockNumber, uint32_t* dirEntryIndex, void* addr);

/**
 * @brief Get an index of a free directory entry in folder's file and save the provided directory entry.
 * It can allocate additional block of data if it is needed.
 * 
 * @param blockNumber - index of a block where the folder's file begins.
 * @param dirEntryIndex - pointer where the index of a free directory entry will be saved.
 * @param dirEntryToSave - pointer to the directory entry structure that will be saved.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * -1 if it failed to allocate additional block of data. It does not found free directory entry.
 */
int8_t fs_occupy_free_dir_entry(uint32_t blockNumber, uint32_t* dirEntryIndex, struct DirEntry* dirEntryToSave, void* addr); 


#endif