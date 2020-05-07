/*
 * test_block_links.c
 *
 *      Author: Kordowski Mateusz
 */


#ifndef SIMPLEFS_BLOCK_LINKS_TEST_C
#define SIMPLEFS_BLOCK_LINKS_TEST_C

#include <sys/mman.h>
#include <sys/stat.h> 
#include <fcntl.h> 

#include "unity.h"
#include "superblock.h"
#include "block_links.h"

void* shm_addr = NULL;
const uint32_t sizeof_shm = 33554432; //(32 MB)
const char* shm_name = "shm_test_block_links";

uint32_t maxOpenFiles = 1024;
uint32_t maxInodes = UINT16_MAX;
uint32_t maxFilesystemSize = sizeof_shm;
uint32_t sizeofOneBlock = 1024;

void setUp_block_links(void){
    // get shm fd
    int fd = shm_open(shm_name, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) errExit(shm_name);

    // allocate memory in shm 
    if (ftruncate(fd, sizeof_shm) == -1) errExit("ftruncate");

    // map the object into the caller's address space
    shm_addr = mmap(NULL, sizeof_shm, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shm_addr == MAP_FAILED) errExit("mmap");

    struct Superblock toSet;

    toSet.max_number_of_inodes = maxInodes;
    toSet.max_number_of_open_files = maxOpenFiles;
    toSet.filesystem_checks = 0;
    toSet.data_block_size = sizeofOneBlock; 
    toSet.number_of_data_blocks = calculate_fs_needed_blocks(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock);
    toSet.fs_size = maxFilesystemSize; 

    toSet.open_file_table_pointer = calculate_fs_superblock_end();
    toSet.open_file_bitmap_pointer = calculate_fs_open_file_table_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock);

    toSet.inode_table_pointer = calculate_fs_open_file_stat_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock);
    toSet.inode_bitmap_pointer = calculate_fs_inode_table_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock);

    toSet.block_links_pointer = calculate_fs_inode_stat_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock);
    toSet.block_bitmap_pointer = calculate_fs_block_links_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock);
    toSet.data_blocks_pointer = calculate_fs_block_stat_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock);

    fs_create_superblock_in_shm(&toSet, shm_addr);

    // create open files structures
    fs_create_blocks_stuctures_in_shm(shm_addr);
}

void tearDown_block_links(void) {
    munmap(shm_addr, sizeof_shm);
    shm_unlink(shm_name);
}

void block_links_getters_setters_test(void){
    uint32_t sizeofData1 = 5000;
    uint32_t sizeofData2 = 10000;
    uint32_t sizeofData3 = 1024*1024*2;
    uint32_t sizeofData4 = 62;

    // may be trash data, does not matter
    void* dataToSave1 = malloc(sizeofData1);
    void* dataToSave2 = malloc(sizeofData2);
    void* dataToSave3 = malloc(sizeofData3);
    void* dataToSave4 = malloc(sizeofData4);

    void* dataToReceived1 = malloc(sizeofData1);
    void* dataToReceived2 = malloc(sizeofData2);
    void* dataToReceived3 = malloc(sizeofData3);
    void* dataToReceived4 = malloc(sizeofData4);

    uint32_t freeBlock1 = fs_allocate_new_chain(shm_addr);
    uint32_t freeBlock2 = fs_allocate_new_chain(shm_addr);
    uint32_t freeBlock3 = fs_allocate_new_chain(shm_addr);
    uint32_t freeBlock4 = fs_allocate_new_chain(shm_addr);

    TEST_ASSERT_TRUE(freeBlock1 != FS_EMPTY_BLOCK_VALUE);
    TEST_ASSERT_TRUE(freeBlock2 != FS_EMPTY_BLOCK_VALUE);
    TEST_ASSERT_TRUE(freeBlock3 != FS_EMPTY_BLOCK_VALUE);
    TEST_ASSERT_TRUE(freeBlock4 != FS_EMPTY_BLOCK_VALUE);

    // memory comparison
    TEST_ASSERT_TRUE(fs_save_data(0, sizeofData1, freeBlock1, dataToSave1, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_get_data(0, sizeofData1, freeBlock1, dataToReceived1, shm_addr));
    TEST_ASSERT_TRUE(memcmp(dataToSave1, dataToReceived1, sizeofData1) == 0);

    TEST_ASSERT_TRUE(fs_save_data(57, sizeofData2, freeBlock2, dataToSave2, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_get_data(57, sizeofData2, freeBlock2, dataToReceived2, shm_addr));
    TEST_ASSERT_TRUE(memcmp(dataToSave2, dataToReceived2, sizeofData2) == 0);

    TEST_ASSERT_TRUE(fs_save_data(3569, sizeofData3, freeBlock3, dataToSave3, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_get_data(3569, sizeofData3, freeBlock3, dataToReceived3, shm_addr));
    TEST_ASSERT_TRUE(memcmp(dataToSave3, dataToReceived3, sizeofData3) == 0);

    TEST_ASSERT_TRUE(fs_save_data(12000, sizeofData4, freeBlock4, dataToSave4, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_get_data(12000, sizeofData4, freeBlock4, dataToReceived4, shm_addr));
    TEST_ASSERT_TRUE(memcmp(dataToSave4, dataToReceived4, sizeofData4) == 0);

    fs_free_blockchain(freeBlock1, shm_addr);
    fs_free_blockchain(freeBlock2, shm_addr);
    fs_free_blockchain(freeBlock3, shm_addr);
    fs_free_blockchain(freeBlock4, shm_addr);

    free(dataToSave1);
    free(dataToSave2);
    free(dataToSave3);
    free(dataToSave4);

    free(dataToReceived1);
    free(dataToReceived2);
    free(dataToReceived3);
    free(dataToReceived4);
}

void block_links_allocation_test(void){
    uint32_t chain1 = fs_allocate_new_chain(shm_addr); // 4
    uint32_t chain2 = fs_allocate_new_chain(shm_addr); // 1
    uint32_t chain3 = fs_allocate_new_chain(shm_addr); // 2
    uint32_t chain4 = fs_allocate_new_chain(shm_addr); // 3

    uint32_t test1, test2, test3, test4, test5, test6, test7, test8, test9, test10;

    TEST_ASSERT_TRUE(chain1 != FS_EMPTY_BLOCK_VALUE);
    TEST_ASSERT_TRUE(chain2 != FS_EMPTY_BLOCK_VALUE);
    TEST_ASSERT_TRUE(chain3 != FS_EMPTY_BLOCK_VALUE);
    TEST_ASSERT_TRUE(chain4 != FS_EMPTY_BLOCK_VALUE);

    TEST_ASSERT_TRUE((test1 = fs_allocate_new_block(chain1, shm_addr)) != FS_EMPTY_BLOCK_VALUE);
    TEST_ASSERT_TRUE((test2 = fs_allocate_new_block(chain2, shm_addr)) != FS_EMPTY_BLOCK_VALUE);
    TEST_ASSERT_TRUE((test3 = fs_allocate_new_block(chain1, shm_addr)) != FS_EMPTY_BLOCK_VALUE);
    TEST_ASSERT_TRUE((test4 = fs_allocate_new_block(chain1, shm_addr)) != FS_EMPTY_BLOCK_VALUE);
    TEST_ASSERT_TRUE((test5 = fs_allocate_new_block(chain3, shm_addr)) != FS_EMPTY_BLOCK_VALUE);
    TEST_ASSERT_TRUE((test6 = fs_allocate_new_block(chain4, shm_addr)) != FS_EMPTY_BLOCK_VALUE);
    TEST_ASSERT_TRUE((test7 = fs_allocate_new_block(chain4, shm_addr)) != FS_EMPTY_BLOCK_VALUE);
    TEST_ASSERT_TRUE((test8 = fs_allocate_new_block(chain4, shm_addr)) != FS_EMPTY_BLOCK_VALUE);
    TEST_ASSERT_TRUE((test9 = fs_allocate_new_block(chain1, shm_addr)) != FS_EMPTY_BLOCK_VALUE);
    TEST_ASSERT_TRUE((test10 = fs_allocate_new_block(chain3, shm_addr)) != FS_EMPTY_BLOCK_VALUE);

    uint32_t next1[4] = {test1, test3, test4, test9};
    uint32_t next2[1] = {test2};
    uint32_t next3[2] = {test5, test10};
    uint32_t next4[3] = {test6, test7, test8};

    // check if chain is properly created
    uint32_t tmp = chain1;
    for(uint32_t i = 0; i < 4; ++i){
        TEST_ASSERT_TRUE((tmp = fs_get_next_block_number(tmp, shm_addr)) != FS_EMPTY_BLOCK_VALUE);
        TEST_ASSERT_TRUE(next1[i] == tmp);
    }
    TEST_ASSERT_TRUE((tmp = fs_get_next_block_number(tmp, shm_addr)) == FS_EMPTY_BLOCK_VALUE);

    tmp = chain2;
    for(uint32_t i = 0; i < 1; ++i){
        TEST_ASSERT_TRUE((tmp = fs_get_next_block_number(tmp, shm_addr)) != FS_EMPTY_BLOCK_VALUE);
        TEST_ASSERT_TRUE(next2[i] == tmp);
    }
    TEST_ASSERT_TRUE((tmp = fs_get_next_block_number(tmp, shm_addr)) == FS_EMPTY_BLOCK_VALUE);

    tmp = chain3;
    for(uint32_t i = 0; i < 2; ++i){
        TEST_ASSERT_TRUE((tmp = fs_get_next_block_number(tmp, shm_addr)) != FS_EMPTY_BLOCK_VALUE);
        TEST_ASSERT_TRUE(next3[i] == tmp);
    }
    TEST_ASSERT_TRUE((tmp = fs_get_next_block_number(tmp, shm_addr)) == FS_EMPTY_BLOCK_VALUE);

    tmp = chain4;
    for(uint32_t i = 0; i < 3; ++i){
        TEST_ASSERT_TRUE((tmp = fs_get_next_block_number(tmp, shm_addr)) != FS_EMPTY_BLOCK_VALUE);
        TEST_ASSERT_TRUE(next4[i] == tmp);
    }
    TEST_ASSERT_TRUE((tmp = fs_get_next_block_number(tmp, shm_addr)) == FS_EMPTY_BLOCK_VALUE);

    fs_free_blockchain(chain1, shm_addr);
    fs_free_blockchain(chain2, shm_addr);
    fs_free_blockchain(chain3, shm_addr);
    fs_free_blockchain(chain4, shm_addr);

    TEST_ASSERT_TRUE(fs_get_next_block_number(chain1, shm_addr) == FS_EMPTY_BLOCK_VALUE);
    TEST_ASSERT_TRUE(fs_get_next_block_number(chain2, shm_addr) == FS_EMPTY_BLOCK_VALUE);
    TEST_ASSERT_TRUE(fs_get_next_block_number(chain3, shm_addr) == FS_EMPTY_BLOCK_VALUE);
    TEST_ASSERT_TRUE(fs_get_next_block_number(chain4, shm_addr) == FS_EMPTY_BLOCK_VALUE);

}

void block_links_all_test(void){
    setUp_block_links();
    block_links_allocation_test();
    block_links_getters_setters_test();
    tearDown_block_links();
}

#endif