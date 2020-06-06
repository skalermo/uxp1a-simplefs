/*
 * test_memory_inode.c
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

void inode_save_get_copy(void){
    uint8_t data8_1 = 0;
    uint8_t data8_2 = 1;

    uint16_t data16_1 = 2;
    uint16_t data16_2 = 3;
    uint16_t data16_3 = 4;

    uint32_t data32 = 5;

    for(uint32_t i = 2; i < MAX_INODES; ++i){
        TEST_ASSERT_EQUAL(0, fs_save_data_to_inode_uint8(i, 4, data8_1, shm_addr));
        TEST_ASSERT_EQUAL(0, fs_save_data_to_inode_uint8(i, 5, data8_2, shm_addr));

        TEST_ASSERT_EQUAL(0, fs_save_data_to_inode_uint16(i, 1, data16_1, shm_addr));
        TEST_ASSERT_EQUAL(0, fs_save_data_to_inode_uint16(i, 2, data16_2, shm_addr));
        TEST_ASSERT_EQUAL(0, fs_save_data_to_inode_uint16(i, 3, data16_3, shm_addr));

        TEST_ASSERT_EQUAL(0, fs_save_data_to_inode_uint32(i, 0, data32, shm_addr));

        data8_1 = (data8_1 + 1) % UINT8_MAX;
        data8_2 = (data8_2 + 1) % UINT8_MAX;

        data16_1 = (data16_1 + 1) % UINT16_MAX;
        data16_2 = (data16_2 + 1) % UINT16_MAX;
        data16_3 = (data16_3 + 1) % UINT16_MAX;

        data32 = (data32 + 1) % UINT32_MAX;
    }

    data8_1 = 0;
    data8_2 = 1;

    data16_1 = 2;
    data16_2 = 3;
    data16_3 = 4;

    data32 = 5;

    uint8_t ret8;
    uint16_t ret16;
    uint32_t ret32;

    for(uint32_t i = 2; i < MAX_INODES; ++i){
        TEST_ASSERT_EQUAL(0, fs_get_data_from_inode_uint8(i, 4, &ret8, shm_addr));
        TEST_ASSERT_EQUAL(data8_1, ret8);
        TEST_ASSERT_EQUAL(0, fs_get_data_from_inode_uint8(i, 5, &ret8, shm_addr));
        TEST_ASSERT_EQUAL(data8_2, ret8);

        TEST_ASSERT_EQUAL(0, fs_get_data_from_inode_uint16(i, 1, &ret16, shm_addr));
        TEST_ASSERT_EQUAL(data16_1, ret16);
        TEST_ASSERT_EQUAL(0, fs_get_data_from_inode_uint16(i, 2, &ret16, shm_addr));
        TEST_ASSERT_EQUAL(data16_2, ret16);
        TEST_ASSERT_EQUAL(0, fs_get_data_from_inode_uint16(i, 3, &ret16, shm_addr));
        TEST_ASSERT_EQUAL(data16_3, ret16);

        TEST_ASSERT_EQUAL(0, fs_get_data_from_inode_uint32(i, 0, &ret32, shm_addr));
        TEST_ASSERT_EQUAL(data32, ret32);


        data8_1 = (data8_1 + 1) % UINT8_MAX;
        data8_2 = (data8_2 + 1) % UINT8_MAX;

        data16_1 = (data16_1 + 1) % UINT16_MAX;
        data16_2 = (data16_2 + 1) % UINT16_MAX;
        data16_3 = (data16_3 + 1) % UINT16_MAX;

        data32 = (data32 + 1) % UINT32_MAX;
    }

    struct Inode inode_cpy;
    
    data8_1 = 0;
    data8_2 = 1;

    data16_1 = 2;
    data16_2 = 3;
    data16_3 = 4;

    data32 = 5;


    for(uint32_t i = 2; i < MAX_INODES; ++i){
        TEST_ASSERT_EQUAL(0, fs_get_inode_copy(i, &inode_cpy, shm_addr));
        TEST_ASSERT_EQUAL(data32, inode_cpy.block_index);
        TEST_ASSERT_EQUAL(data16_1, inode_cpy.file_size);
        TEST_ASSERT_EQUAL(data16_2, inode_cpy.readers);
        TEST_ASSERT_EQUAL(data16_3, inode_cpy.writers);
        TEST_ASSERT_EQUAL(data8_1, inode_cpy.mode);
        TEST_ASSERT_EQUAL(data8_2, inode_cpy.ref_count);
        


        data8_1 = (data8_1 + 1) % UINT8_MAX;
        data8_2 = (data8_2 + 1) % UINT8_MAX;

        data16_1 = (data16_1 + 1) % UINT16_MAX;
        data16_2 = (data16_2 + 1) % UINT16_MAX;
        data16_3 = (data16_3 + 1) % UINT16_MAX;

        data32 = (data32 + 1) % UINT32_MAX;
    }

    // check inode number 1
    TEST_ASSERT_EQUAL(0, fs_get_inode_copy(1, &inode_cpy, shm_addr));

    TEST_ASSERT_EQUAL(0, inode_cpy.block_index);
    TEST_ASSERT_EQUAL(BLOCK_SIZE, inode_cpy.file_size);
    TEST_ASSERT_EQUAL(0, inode_cpy.readers);
    TEST_ASSERT_EQUAL(0, inode_cpy.writers);
    TEST_ASSERT_EQUAL(IS_DIR, inode_cpy.mode);
    TEST_ASSERT_EQUAL(0, inode_cpy.ref_count);

    // check if inodes are in right place in memory
    memcpy(&inode_cpy, shm_addr + 8238 + sizeof(struct Inode), sizeof(struct Inode));
    TEST_ASSERT_EQUAL(0, inode_cpy.block_index);
    TEST_ASSERT_EQUAL(BLOCK_SIZE, inode_cpy.file_size);
    TEST_ASSERT_EQUAL(0, inode_cpy.readers);
    TEST_ASSERT_EQUAL(0, inode_cpy.writers);
    TEST_ASSERT_EQUAL(IS_DIR, inode_cpy.mode);
    TEST_ASSERT_EQUAL(0, inode_cpy.ref_count);

    if(data8_1 == 0) data8_1 = UINT8_MAX - 1;
    else --data8_1;

    if(data8_2 == 0) data8_2 = UINT8_MAX - 1;
    else --data8_2;

    if(data16_1 == 0) data16_1 = UINT16_MAX - 1;
    else --data16_1;

    if(data16_2 == 0) data16_2 = UINT16_MAX - 1;
    else --data16_2;

    if(data16_3 == 0) data16_3 = UINT16_MAX - 1;
    else --data16_3;

    if(data32 == 0) data32 = UINT32_MAX - 1;
    else --data32;


    memcpy(&inode_cpy, shm_addr + 1056798 - sizeof(struct Inode), sizeof(struct Inode));
    TEST_ASSERT_EQUAL(data32, inode_cpy.block_index);
    TEST_ASSERT_EQUAL(data16_1, inode_cpy.file_size);
    TEST_ASSERT_EQUAL(data16_2, inode_cpy.readers);
    TEST_ASSERT_EQUAL(data16_3, inode_cpy.writers);
    TEST_ASSERT_EQUAL(data8_1, inode_cpy.mode);
    TEST_ASSERT_EQUAL(data8_2, inode_cpy.ref_count);
    
    
}

void inode_bitmap(){
    uint16_t inodeIndex = 0;
    struct Inode inode = {};

    for(uint32_t i = 2; i < MAX_INODES - 1; ++i){
        TEST_ASSERT_EQUAL(0, fs_get_free_inode(&inodeIndex, shm_addr));
        TEST_ASSERT_EQUAL(i, inodeIndex);
        TEST_ASSERT_EQUAL(0, fs_occupy_free_inode(&inodeIndex, &inode, shm_addr));
        TEST_ASSERT_EQUAL(0, fs_get_free_inode(&inodeIndex, shm_addr));
        TEST_ASSERT_EQUAL(i + 1, inodeIndex);
        TEST_ASSERT_EQUAL(i + 1, fs_get_used_inodes(shm_addr));
    }

    // for last inode
    TEST_ASSERT_EQUAL(0, fs_get_free_inode(&inodeIndex, shm_addr));
    TEST_ASSERT_EQUAL(MAX_INODES - 1, inodeIndex);
    TEST_ASSERT_EQUAL(0, fs_occupy_free_inode(&inodeIndex, &inode, shm_addr));
    TEST_ASSERT_EQUAL(MAX_INODES, fs_get_used_inodes(shm_addr));
    TEST_ASSERT_EQUAL(-1, fs_get_free_inode(&inodeIndex, shm_addr));

    for(uint32_t i = 2; i < MAX_INODES - 1; ++i){
        TEST_ASSERT_EQUAL(0, fs_mark_inode_as_free(i, shm_addr));
        TEST_ASSERT_EQUAL(0, fs_get_free_inode(&inodeIndex, shm_addr));
        TEST_ASSERT_EQUAL(i, inodeIndex);

        TEST_ASSERT_EQUAL(0, fs_mark_inode_as_used(i, shm_addr));
        TEST_ASSERT_EQUAL(-1, fs_get_free_inode(&inodeIndex, shm_addr));

        TEST_ASSERT_EQUAL(MAX_INODES, fs_get_used_inodes(shm_addr));
    }

    TEST_ASSERT_EQUAL(0, fs_set_used_inodes(569, shm_addr));
    TEST_ASSERT_EQUAL(569, fs_get_used_inodes(shm_addr));
    TEST_ASSERT_EQUAL(0, fs_set_used_inodes(MAX_INODES, shm_addr));
    TEST_ASSERT_EQUAL(MAX_INODES, fs_get_used_inodes(shm_addr));

    for(uint32_t i = MAX_INODES - 1; i >= 2; --i){
        TEST_ASSERT_EQUAL(0, fs_mark_inode_as_free(i, shm_addr));
        TEST_ASSERT_EQUAL(0, fs_get_free_inode(&inodeIndex, shm_addr));
        TEST_ASSERT_EQUAL(i, inodeIndex);
    }
}


int main(void){
    UNITY_BEGIN();

    RUN_TEST(inode_save_get_copy);
    RUN_TEST(inode_bitmap);
    
    return UNITY_END();
}
