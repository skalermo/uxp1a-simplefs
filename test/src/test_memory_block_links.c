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
    uint32_t* chains = malloc((fs_get_max_number_data_blocks(shm_addr) / chainSize) + 1);
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
}

int main(void){
    UNITY_BEGIN();

    RUN_TEST(block_links_allocate);
    RUN_TEST(block_links_allocate_chain);
    
    return UNITY_END();
}
