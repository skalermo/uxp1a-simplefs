/*
 * block_links.c
 *
 *      Author: Kordowski Mateusz
 */

#ifndef SIMPLEFS_BLOCK_LINKS_C
#define SIMPLEFS_BLOCK_LINKS_C

#include "block_links.h"
#include "utils.c"


///////////////////////////////////
//  Struct functions
//////////////////////////////////


int8_t fs_get_data(uint32_t from, uint32_t to, uint32_t initialBlockNumber, void* receivedData, void* addr){
    uint32_t nextBlockIndex = initialBlockNumber;
    uint16_t blockSize = fs_get_data_block_size(addr);
    
    uint32_t howManyBlocks = (to - from) / blockSize;
    uint32_t lastBlockOccupancy = (to - from) % blockSize;

    uint32_t beginBlock = from / blockSize;
    uint32_t beginBlockOccupancy = from % blockSize;

    uint32_t receivedData_ptr = 0;

    // loop to the block where you start reading
    for(uint32_t i = 0; i < beginBlock; ++i){
        if(inner_fs_next_block_with_error(nextBlockIndex, addr) == -1) return -1;
    }

    // first block
    void* dataBlockReal_ptr = fs_get_data_blocks_ptr(addr) + (nextBlockIndex * blockSize);
    if(inner_fs_next_block_with_error(nextBlockIndex, addr) == -1) return -1;
    uint32_t differenceInBlock = blockSize - beginBlockOccupancy;

    memcpy(receivedData, dataBlockReal_ptr + beginBlockOccupancy, differenceInBlock);
    receivedData_ptr += differenceInBlock;

    // rest of blocks
    differenceInBlock = blockSize;
    for(uint32_t i = 1; i < howManyBlocks; ++i){
        dataBlockReal_ptr = fs_get_data_blocks_ptr(addr) + (nextBlockIndex * blockSize);
        if(inner_fs_next_block_with_error(nextBlockIndex, addr) == -1) return -1;

        memcpy(receivedData + receivedData_ptr, dataBlockReal_ptr, differenceInBlock);
        receivedData_ptr += differenceInBlock;
    }

    // end of read, possibly last block
    if(lastBlockOccupancy != 0){
        differenceInBlock = (to - from) - receivedData_ptr;
        dataBlockReal_ptr = fs_get_data_blocks_ptr(addr) + (nextBlockIndex * blockSize);
        memcpy(receivedData + receivedData_ptr, dataBlockReal_ptr, differenceInBlock);
    }

    return 0;
}

int8_t fs_save_data(uint32_t from, uint32_t to, uint32_t initialBlockNumber, void* dataToSave, void* addr){
    uint32_t nextBlockIndex = initialBlockNumber;
    uint16_t blockSize = fs_get_data_block_size(addr);
    
    uint32_t howManyBlocks = (to - from) / blockSize;
    uint32_t lastBlockOccupancy = (to - from) % blockSize;

    uint32_t beginBlock = from / blockSize;
    uint32_t beginBlockOccupancy = from % blockSize;

    uint32_t recordData_ptr = 0;

     // loop to the block where you start reading
    for(uint32_t i = 0; i < beginBlock; ++i){
        if(inner_fs_next_block_with_allocate(nextBlockIndex, addr) == -1) return -1;
    }

    // first block
    void* dataBlockReal_ptr = fs_get_data_blocks_ptr(addr) + (nextBlockIndex * blockSize);
    if(inner_fs_next_block_with_allocate(nextBlockIndex, addr) == -1) return -1;
    uint32_t differenceInBlock = blockSize - beginBlockOccupancy;

    memcpy(dataBlockReal_ptr + beginBlockOccupancy, dataToSave, differenceInBlock);
    recordData_ptr += differenceInBlock;

    // rest of blocks
    differenceInBlock = blockSize;
    for(uint32_t i = 1; i < howManyBlocks; ++i){
        dataBlockReal_ptr = fs_get_data_blocks_ptr(addr) + (nextBlockIndex * blockSize);
        if(inner_fs_next_block_with_allocate(nextBlockIndex, addr) == -1) return -1;

        memcpy(dataBlockReal_ptr, dataToSave + recordData_ptr, differenceInBlock);
        recordData_ptr += differenceInBlock;
    }

    // end of read, possibly last block
    if(lastBlockOccupancy != 0){
        differenceInBlock = (to - from) - recordData_ptr;
        dataBlockReal_ptr = fs_get_data_blocks_ptr(addr) + (nextBlockIndex * blockSize);
        memcpy(dataBlockReal_ptr, dataToSave + recordData_ptr, differenceInBlock);
    }

    return 0;
}

uint32_t fs_get_next_block_number(uint32_t blockNumber, void* addr){
    void* block_ptr = fs_get_block_links_ptr(addr);
    uint32_t nextBlockNumber;
    memcpy(&nextBlockNumber, block_ptr + (blockNumber * sizeof(uint32_t)));

    return nextBlockNumber;
}

uint32_t fs_allocate_new_block(uint32_t blockNumerInChain, void* addr){
    void* block_ptr = fs_get_block_links_ptr(addr);
    uint32_t nextBlockNumber = blockNumerInChain;

    do{
        blockNumerInChain = nextBlockNumber;
        memcpy(&nextBlockNumber, block_ptr + (blockNumerInChain * sizeof(uint32_t)));
    }
    while(nextBlockNumber != FS_EMPTY_BLOCK_VALUE);

    uint32_t freeBlockIndex = inner_fs_find_free_index(fs_get_block_bitmap_ptr(addr), FS_NUMBER_OF_BLOCKS);

    if(freeBlockIndex == UINT32_MAX) return FS_EMPTY_BLOCK_VALUE;
    if(freeBlockIndex == 0) return FS_EMPTY_BLOCK_VALUE;

    inner_fs_mark_bitmap_bit(fs_get_block_bitmap_ptr(addr), freeBlockIndex);

    return freeBlockIndex;
}

uint32_t fs_allocate_new_chain(void* addr){
    uint32_t freeBlockIndex = inner_fs_find_free_index(fs_get_block_bitmap_ptr(addr), FS_NUMBER_OF_BLOCKS);

    if(freeBlockIndex == UINT32_MAX) return FS_EMPTY_BLOCK_VALUE;
    if(freeBlockIndex == 0) return FS_EMPTY_BLOCK_VALUE;

    inner_fs_mark_bitmap_bit(fs_get_block_bitmap_ptr(addr), freeBlockIndex);

    return freeBlockIndex;
}

uint8_t fs_free_blockchain(uint32_t firstBlockInBlockchain, void* addr){
    if(firstBlockInBlockchain == FS_EMPTY_BLOCK_VALUE) return 0;

    uint32_t nextBlockIndex;
    uint32_t reset = FS_EMPTY_BLOCK_VALUE;

    do{
        nextBlockIndex = fs_get_next_block_number(firstBlockInBlockchain, addr);

        inner_fs_free_bitmap_bit(fs_get_block_bitmap_ptr(addr), firstBlockInBlockchain);
        memcpy(fs_get_block_links_ptr(addr) + (sizeof(uint32_t) * firstBlockInBlockchain), reset, sizeof(uint32_t));

        firstBlockInBlockchain = nextBlockIndex;
    }
    while(firstBlockInBlockchain != FS_EMPTY_BLOCK_VALUE);

    return 0;
}

int8_t fs_create_blocks_stuctures_in_shm(void* addr){
    void* blockLinks_ptr = fs_get_block_links_ptr(addr);
    void* blockStat_ptr = fs_get_block_bitmap_ptr(addr);

    struct BlockLinks* toSave = malloc(sizeof(struct BlockLinks));
    struct BlockStat* toSaveStat = malloc(sizeof(struct BlockStat));

    for(unsigned int i = 0; i < FS_NUMBER_OF_BLOCKS; ++i){
        toSave->block_num[0] = FS_EMPTY_BLOCK_VALUE;
    }

    // first block is for main directory
    toSaveStat->block_bitmap[0] = 0x7F;

    for(unsigned int i = 1; i < FS_NUMBER_OF_BLOCKS_BY_8; ++i){
        toSaveStat->block_bitmap[i] = 0xFF;
    }
    toSaveStat->used_data_blocks = 0;

    memcpy(blockLinks_ptr, toSave, sizeof(struct BlockLinks));
    memcpy(blockStat_ptr, toSaveStat, sizeof(struct BlockStat));

    free(toSaveStat);
    free(toSave);
    return 0;
}

///////////////////////////////////////////////
//  Private functions (used in dir_file too)
//////////////////////////////////////////////

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
