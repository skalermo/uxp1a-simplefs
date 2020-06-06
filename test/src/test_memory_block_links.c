/*
 * test_memory_block_links.c
 *
 *      Author: Kordowski Mateusz
 */


#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/stat.h> 
#include <fcntl.h> 

#include "unity.h"
#include "simplefs_utils.h"
#include "init.h"

void* shm_addr = NULL;

void setUp(void){
    shm_addr = get_ptr_to_fs();
}

void tearDown(void){
    unlink_fs();
}

void block_links_allocate(void){
    uint32_t block;

    for(uint32_t i = 1; i < fs_get_max_number_data_blocks(shm_addr); ++i){
        TEST_ASSERT_EQUAL(i, fs_get_used_blocks(shm_addr));
        TEST_ASSERT_EQUAL(i, block = fs_allocate_new_chain(shm_addr));
        TEST_ASSERT_EQUAL(i + 1, fs_get_used_blocks(shm_addr));
    }

    for(uint32_t i = fs_get_max_number_data_blocks(shm_addr) - 1; i >= 1; --i){
        TEST_ASSERT_EQUAL(i + 1, fs_get_used_blocks(shm_addr));
        TEST_ASSERT_EQUAL(0, fs_free_blockchain(i, shm_addr));
        TEST_ASSERT_EQUAL(i, fs_get_used_blocks(shm_addr));
    }

    

}

void block_links_allocate_chain(void){
    uint32_t block;
    uint32_t chainSize = 80;
    uint32_t prevBlock;
    uint32_t* chains = malloc(fs_get_max_number_data_blocks(shm_addr));
    uint32_t chainIndex = 0;

    for(uint32_t i = 1; i < fs_get_max_number_data_blocks(shm_addr); ++i){
        TEST_ASSERT_EQUAL(i, fs_get_used_blocks(shm_addr));
        TEST_ASSERT_EQUAL(i, block = fs_allocate_new_chain(shm_addr));
        chains[chainIndex++] = block;
        TEST_ASSERT_EQUAL(++i, fs_get_used_blocks(shm_addr));

        for(uint32_t j = 0; i < fs_get_max_number_data_blocks(shm_addr) && j < chainSize; ++i, ++j){
            TEST_ASSERT_EQUAL(i, fs_get_used_blocks(shm_addr));
            TEST_ASSERT_EQUAL(i, prevBlock = fs_allocate_new_block(block, shm_addr));
            TEST_ASSERT_EQUAL(i + 1, fs_get_used_blocks(shm_addr));
        }
        --i;
    }

    uint32_t ind = 1;

    for(; ind < fs_get_max_number_data_blocks(shm_addr) - 1; ++ind){

        for(uint32_t j = 0; ind < fs_get_max_number_data_blocks(shm_addr) - 1 && j < chainSize - 1; ++ind, ++j){
            TEST_ASSERT_EQUAL(ind + 1, block = fs_get_next_block_number(ind, shm_addr));
        }
        if(ind == fs_get_max_number_data_blocks(shm_addr) - 1) break;
        ++ind;
        TEST_ASSERT_EQUAL(FS_EMPTY_BLOCK_VALUE, block = fs_get_next_block_number(ind, shm_addr));
    }
    TEST_ASSERT_EQUAL(FS_EMPTY_BLOCK_VALUE, block = fs_get_next_block_number(ind, shm_addr));

    uint32_t usedBlocks = fs_get_used_blocks(shm_addr);

    for(uint32_t i = 0; i < chainIndex; ++i){
        TEST_ASSERT_EQUAL(0, fs_free_blockchain(chains[i], shm_addr));
        if(usedBlocks >= chainSize + 1) usedBlocks -= chainSize + 1;
        else usedBlocks = 1;

        TEST_ASSERT_EQUAL(usedBlocks, fs_get_used_blocks(shm_addr));
    }

    free(chains);
}

void block_links_saving(){
    uint32_t numberOfInt = 943;
    uint32_t sizeofData = sizeof(uint32_t) * numberOfInt;
    uint32_t* dataToSave = malloc(sizeofData);
    uint32_t* receivedData = malloc(sizeofData);
    uint32_t numberOfFiles = 6510;
    uint32_t offsetBytes = 9863;
    uint32_t* chains = malloc(numberOfFiles * sizeof(uint32_t));

    for(uint32_t i = 0; i < numberOfInt; ++i){
        dataToSave[i] = i;
    }

    for(uint32_t i = 0; i < numberOfFiles - 1; ++i){
        chains[i] = fs_allocate_new_chain(shm_addr);
        TEST_ASSERT_EQUAL(0, fs_save_data(0, sizeofData, chains[i], dataToSave, shm_addr));
        ++i;
        chains[i] = fs_allocate_new_chain(shm_addr);
        TEST_ASSERT_EQUAL(0, fs_save_data(offsetBytes, offsetBytes + sizeofData, chains[i], dataToSave, shm_addr));
    }

    for(uint32_t i = 0; i < numberOfFiles - 1; ++i){
        TEST_ASSERT_EQUAL(0, fs_get_data(0, sizeofData, chains[i], receivedData, shm_addr));
        TEST_ASSERT_EQUAL_UINT32_ARRAY(dataToSave, receivedData, numberOfInt);
        ++i;
        TEST_ASSERT_EQUAL(0, fs_get_data(offsetBytes, offsetBytes + sizeofData, chains[i], receivedData, shm_addr));
        TEST_ASSERT_EQUAL_UINT32_ARRAY(dataToSave, receivedData, numberOfInt);
    }

    free(chains);
    free(dataToSave);
    free(receivedData);
}

void block_links_saving_count(){
    uint32_t numberOfInt = 769; // 943
    uint32_t sizeofData = sizeof(uint32_t) * numberOfInt;
    uint32_t* dataToSave = malloc(sizeofData);
    uint32_t* receivedData = malloc(sizeofData);
    uint32_t numberOfFiles = 6510;
    uint32_t offsetBytes = 9863;
    uint32_t* chains = malloc(numberOfFiles * sizeof(uint32_t));
    uint32_t boolean = ((sizeofData % BLOCK_SIZE) != 0);
    uint32_t boolean2 = (((offsetBytes + sizeofData) % BLOCK_SIZE) != 0);

    for(uint32_t i = 0; i < numberOfInt; ++i){
        dataToSave[i] = i;
    }

    for(uint32_t i = 0; i < numberOfFiles - 1; ++i){
        chains[i] = fs_allocate_new_chain(shm_addr);
        TEST_ASSERT_EQUAL(sizeofData, fs_save_data_count(0, sizeofData, chains[i], dataToSave, shm_addr));

        uint32_t block = chains[i];
        for(uint32_t j = 0; j < (sizeofData / BLOCK_SIZE) + boolean - 1; ++j){
            TEST_ASSERT_NOT_EQUAL(FS_EMPTY_BLOCK_VALUE, block = fs_get_next_block_number(block, shm_addr));
        }
        TEST_ASSERT_EQUAL(FS_EMPTY_BLOCK_VALUE, block = fs_get_next_block_number(block, shm_addr));

        chains[++i] = fs_allocate_new_chain(shm_addr);
        block = chains[i];
        TEST_ASSERT_EQUAL(sizeofData, fs_save_data_count(offsetBytes, offsetBytes + sizeofData, chains[i], dataToSave, shm_addr));
        for(uint32_t j = 0; j < ((offsetBytes + sizeofData) / BLOCK_SIZE) + boolean2 - 1; ++j){
            TEST_ASSERT_NOT_EQUAL(FS_EMPTY_BLOCK_VALUE, block = fs_get_next_block_number(block, shm_addr));
        }
        TEST_ASSERT_EQUAL(FS_EMPTY_BLOCK_VALUE, block = fs_get_next_block_number(block, shm_addr));
    }

    for(uint32_t i = 0; i < numberOfFiles - 1; ++i){
        TEST_ASSERT_EQUAL(sizeofData, fs_get_data_count(0, sizeofData, chains[i], receivedData, shm_addr));
        TEST_ASSERT_EQUAL_UINT32_ARRAY(dataToSave, receivedData, numberOfInt);
        ++i;
        TEST_ASSERT_EQUAL(sizeofData, fs_get_data_count(offsetBytes, offsetBytes + sizeofData, chains[i], receivedData, shm_addr));
        TEST_ASSERT_EQUAL_UINT32_ARRAY(dataToSave, receivedData, numberOfInt);
    }

    free(chains);
    free(dataToSave);
    free(receivedData);
}


int main(void){
    UNITY_BEGIN();

    RUN_TEST(block_links_allocate);
    RUN_TEST(block_links_allocate_chain);
    RUN_TEST(block_links_saving);
    RUN_TEST(block_links_saving_count);
    
    return UNITY_END();
}
