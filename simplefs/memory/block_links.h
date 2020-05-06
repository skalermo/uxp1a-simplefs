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
 * 
 * @param from - starting place in blockchain in bytes.
 * @param to - ending place in blockchain in bytes.
 * @param initialBlockNumber - initial block number where the file's data is stored.
 * @param receivedData - pointer where the read data was saved. The allocated memory should be big enough to store read data.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * Otherwise error code.
 */
int8_t fs_get_data(uint32_t from, uint32_t to, uint32_t initialBlockNumber, void* receivedData, void* addr);

/**
 * @brief Save data to the file.
 * @details The data in pointer will be read form the beginning and if the data size is bigger than difference 
 * of the pointers in blockchain then only a part of data equal of this difference will be saved.
 * 
 * @param from - starting place in blockchain.
 * @param to - ending place in blockchain.
 * @param initialBlockNumber - initial block number where the file's data is stored.
 * @param dataToRecord - pointer where the data to save is stored. 
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * Otherwise error code.
 */
int8_t fs_save_data(uint32_t from, uint32_t to, uint32_t initialBlockNumber, void* dataToRecord, void* addr);

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
 * 
 * @param blockNumerInChain - block index where the chain is. It is possible to give a non-initial index of blockchain.
 * @param addr - address of the mapped shared memory.
 * @return uint32_t - FS_EMPTY_BLOCK_VALUE if failed to allocate new block or
 * index of a new alocated block.
 */
uint32_t fs_allocate_new_block(uint32_t blockNumerInChain, void* addr);

/**
 * @brief Allocates the new blockchain. The returned block is marked in bitmap as used.
 * 
 * @param addr - address of the mapped shared memory.
 * @return uint32_t - FS_EMPTY_BLOCK_VALUE if failed to allocate new block or
 * index of a new alocated block.
 */
uint32_t fs_allocate_new_chain(void* addr);

/**
 * @brief Frees alocated memory in this blockchain.
 * It must be given 
 * 
 * @param addr - address of the mapped shared memory.
 * @return uint8_t - 0 if operation was successful.
 * Otherwise error code.
 */
uint8_t fs_free_blockchain(uint32_t firstBlockInBlockchain,void* addr);



///////////////////////////////////
//  Other functions
//////////////////////////////////

/**
 * @brief Creates a initial block links structures in shared memory.
 * 
 * @param offsetTable - offset of a block links table counted from where the superblock is.
 * @param offsetBitmap - offset of a block bitmap structure counted from where the superblock is.
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * Otherwise error code.
 */
int8_t fs_create_blocks_stuctures_in_shm(uint32_t offsetLinks, uint32_t offsetBitmap, void* addr); // TO_CHECK, TODO



///////////////////////////////////
//  Private functions (used in dir_file too)
//////////////////////////////////

int8_t inner_fs_next_block_with_allocate(uint32_t* blockIndex, void* addr){
    uint32_t previousBlockIndex = *blockIndex;
    *blockIndex = fs_get_next_block_number(previousBlockIndex, addr);
    if(*blockIndex == FS_EMPTY_BLOCK_VALUE) {
        *blockIndex = fs_allocate_new_block(previousBlockIndex, addr);
        if(*blockIndex == FS_EMPTY_BLOCK_VALUE) return -1;
        return 1;
    }

    return 0;
}

int8_t inner_fs_next_block_with_error(uint32_t* blockIndex, void* addr){
    *blockIndex = fs_get_next_block_number(*blockIndex, addr);
    if(*blockIndex == FS_EMPTY_BLOCK_VALUE) {
        return -1;
    }

    return 0;
}

#endif