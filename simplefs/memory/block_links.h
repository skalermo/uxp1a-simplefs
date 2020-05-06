/*
 * block_links.h
 *
 *      Author: Kordowski Mateusz
 */

#ifndef SIMPLEFS_BLOCK_LINKS_H
#define SIMPLEFS_BLOCK_LINKS_H

#include <stdint.h>
#include <stddef.h> // offsetof function to use with structs.
#include <limits.h>

#include "superblock.h"

///////////////////////////////////
//  Defines
//////////////////////////////////


#define FS_NUMBER_OF_BLOCKS 65536
#define FS_NUMBER_OF_BLOCKS_BY_8 ((FS_NUMBER_OF_BLOCKS / 8) + 1)
#define FS_BLOCK_SIZE 1024 // in bytes
#define FS_EMPTY_BLOCK_VALUE UINT32_MAX


///////////////////////////////////
//  Structs
//////////////////////////////////

struct BlockLinks{
    uint32_t block_num[FS_NUMBER_OF_BLOCKS];
};

struct BlockStat{
    uint8_t block_bitmap[FS_NUMBER_OF_BLOCKS_BY_8];
    uint32_t used_data_blocks;
};



///////////////////////////////////
//  Struct functions
//////////////////////////////////

/**
 * @brief Get data that was saved in the file system.
 * It does not allocate any more memory.
 * 
 * @param from - starting place in blockchain in bytes.
 * @param to - ending place in blockchain in bytes.
 * @param initialBlockNumber - initial block number where the file's data is stored.
 * @param receivedData - pointer where the read data was saved. The allocated memory should be big enough to store read data.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * -1 if you went beyond the allocated memory.
 */
int8_t fs_get_data(uint32_t from, uint32_t to, uint32_t initialBlockNumber, void* receivedData, void* addr);

/**
 * @brief Save data to the file.
 * @details The data in dataToRecord pointer will be read form the beginning and if the data size is bigger than difference 
 * of the pointers in blockchain then the next blocks will be allocated.
 * 
 * @param from - starting place in blockchain.
 * @param to - ending place in blockchain.
 * @param initialBlockNumber - initial block number where the file's data is stored.
 * @param dataToSave - pointer where the data to save is stored. 
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * -1 if it failed to allocate the next block in blockchain.
 */
int8_t fs_save_data(uint32_t from, uint32_t to, uint32_t initialBlockNumber, void* dataToSave, void* addr);

/**
 * @brief Get the index of the next block in blockchain.
 * 
 * @param blockNumber - the block number in the blockchain.
 * @param addr - address of the mapped shared memory.
 * @return uint32_t - index of the next block or 
 * FS_EMPTY_BLOCK_VALUE if there is no next block in the blockchain.
 */
uint32_t fs_get_next_block_number(uint32_t blockNumber, void* addr);

/**
 * @brief Allocates the new block in given blockchain.
 * The block is marked in bitmap as used.
 * 
 * @param blockNumerInChain - block index where the chain is. It is possible to give a non-initial index of blockchain.
 * @param addr - address of the mapped shared memory.
 * @return uint32_t -index of a new alocated block. 
 * FS_EMPTY_BLOCK_VALUE if failed to allocate new block.
 */
uint32_t fs_allocate_new_block(uint32_t blockNumerInChain, void* addr);

/**
 * @brief Allocates the new blockchain. 
 * The returned block is marked in bitmap as used.
 * 
 * @param addr - address of the mapped shared memory.
 * @return uint32_t - index of a new alocated block.
 * FS_EMPTY_BLOCK_VALUE if failed to allocate new block.
 */
uint32_t fs_allocate_new_chain(void* addr);

/**
 * @brief Frees alocated memory in this blockchain.
 * 
 * @param firstBlockInBlockchain - the first block in blockchain that will be freed
 * @param addr - address of the mapped shared memory.
 * @return uint8_t - 0 if operation was successful.
 * No other errors.
 */
uint8_t fs_free_blockchain(uint32_t firstBlockInBlockchain, void* addr);



///////////////////////////////////
//  Other functions
//////////////////////////////////

/**
 * @brief Creates a initial block links structures in shared memory.
 * Superblock must be already created.
 * 
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * No other errors.
 */
int8_t fs_create_blocks_stuctures_in_shm(void* addr); // TO_CHECK, TODO



///////////////////////////////////////////////
//  Private functions (used in dir_file too)
//////////////////////////////////////////////

/**
 * @brief Get next block in blockchain and if there is no next block then allocate it.
 * 
 * @param blockIndex - checks if this block points to the next block.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if there was next block.
 * 1 if the next block was allocated.
 * -1 if allocation of the next block failed. blockIndex is set to FS_EMPTY_BLOCK_VALUE
 */
int8_t inner_fs_next_block_with_allocate(uint32_t* blockIndex, void* addr);

/**
 * @brief Get next block in blockchain and if there is no next block then return error.
 * 
 * @param blockIndex - checks if this block points to the next block.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if there was next block
 * -1 if there was no next block. blockIndex is set to FS_EMPTY_BLOCK_VALUE
 */
int8_t inner_fs_next_block_with_error(uint32_t* blockIndex, void* addr);

#endif