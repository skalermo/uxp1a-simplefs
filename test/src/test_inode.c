/*
 * test_inode.c
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
#include "superblock.h"
#include "open_files.h"

void* shm_addr = NULL;
const char* shm_name = "shm_test_inodes";

const uint32_t maxOpenFiles = 1024;
const uint32_t maxInodes = UINT16_MAX;
const uint32_t maxFilesystemSize = 33554432; //(32 MB);
const uint32_t sizeofOneBlock = 1024;

void setUp(void){

}

void tearDown(void){

}

void tearDown_inode(void) {
    munmap(shm_addr, maxFilesystemSize);
    shm_unlink(shm_name);
}

void setUp_inode(void){
    tearDown_inode();

    // get shm fd
    int fd = shm_open(shm_name, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        puts("shm failed");
        exit(EXIT_FAILURE);
    }

    // allocate memory in shm 
    if (ftruncate(fd, maxFilesystemSize) == -1) {
        puts("ftruncate failed");
        exit(EXIT_FAILURE);
    }

    // map the object into the caller's address space
    shm_addr = mmap(NULL, maxFilesystemSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shm_addr == MAP_FAILED) {
        puts("mmap failed");
        exit(EXIT_FAILURE);
    }

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
    fs_create_inode_structures_in_shm(shm_addr);
}

void inode_setters_getters_test(void){
    uint16_t inodes[10];
    uint32_t data32 = 565411;
    uint16_t data16 = 4864;
    uint8_t data8 = 26;

    uint8_t offset1 = 11;
    uint8_t offset2 = 22;
    
    uint32_t data32S = data32;
    uint16_t data16S = data16;
    uint8_t data8S1 = data8 + offset1;
    uint8_t data8S2 = data8 + offset2;

    uint32_t data32R;
    uint16_t data16R;
    uint8_t data8R;

    for(uint32_t i = 0; i < 10; ++i){
        TEST_ASSERT_TRUE(fs_get_free_inode(&inodes[i], shm_addr) == 0);
        TEST_ASSERT_TRUE(fs_mark_inode_as_used(inodes[i], shm_addr) == 0);
    }

    TEST_ASSERT_TRUE(inodes[2] != inodes[5] && inodes[7] != inodes[9]);

    for(uint32_t i = 0; i < 10; ++i){
        TEST_ASSERT_TRUE(fs_save_data_to_inode_uint32(inodes[i], 0, data32S, shm_addr) == 0);
        ++data32S;
    }
    
    for(uint32_t i = 0; i < 10; ++i){
        TEST_ASSERT_TRUE(fs_save_data_to_inode_uint16(inodes[i], 1, data16S, shm_addr) == 0);
        ++data16S;
    }

    for(uint32_t i = 0; i < 10; ++i){
        TEST_ASSERT_TRUE(fs_save_data_to_inode_uint8(inodes[i], 4, data8S1, shm_addr) == 0);
        ++data8S1;
        TEST_ASSERT_TRUE(fs_save_data_to_inode_uint8(inodes[i], 5, data8S2, shm_addr) == 0);
        ++data8S2;
    }

    for(uint32_t i = 0; i < 10; ++i){
        TEST_ASSERT_TRUE(fs_get_data_from_inode_uint32(inodes[i], 0, &data32R, shm_addr) == 0);
        TEST_ASSERT_TRUE(data32R == data32 + i);
    }

    for(uint32_t i = 0; i < 10; ++i){
        TEST_ASSERT_TRUE(fs_get_data_from_inode_uint16(inodes[i], 1, &data16R, shm_addr) == 0);
        TEST_ASSERT_TRUE(data16R == data16 + i);
    }

    for(uint32_t i = 0; i < 10; ++i){
        TEST_ASSERT_TRUE(fs_get_data_from_inode_uint8(inodes[i], 4, &data8R, shm_addr) == 0);
        TEST_ASSERT_TRUE(data8R == data8 + i + offset1);

        TEST_ASSERT_TRUE(fs_get_data_from_inode_uint8(inodes[i], 5, &data8R, shm_addr) == 0);
        TEST_ASSERT_TRUE(data8R == data8 + i + offset2);
    }

    struct Inode inod;

    for(uint32_t i = 0; i < 10; ++i){
        TEST_ASSERT_TRUE(fs_get_inode_copy(inodes[i], &inod, shm_addr) == 0);
        TEST_ASSERT_TRUE(inod.block_index == data32 + i);
        TEST_ASSERT_TRUE(inod.file_size == data16 + i);
        TEST_ASSERT_TRUE(inod.mode == data8 + i + offset1);
        TEST_ASSERT_TRUE(inod.ref_count == data8 + i + offset2);
    }
    
}

void inode_count_up(struct Inode* inode){
    ++inode->block_index;
    ++inode->file_size;
    ++inode->mode;
    ++inode->readers;
    ++inode->ref_count;
}

void inode_bitmap_test(void){
    struct Inode toSave;
    toSave.block_index = 45;
    toSave.file_size = 689;
    toSave.mode = 3;
    toSave.readers = 17;
    toSave.ref_count = 55;

    uint16_t inode[10];
    uint16_t tmp;

    for(uint32_t i = 0; i < 10; ++i){
       TEST_ASSERT_TRUE(fs_occupy_free_inode(&inode[i], &toSave, shm_addr) == 0);
       inode_count_up(&toSave);
    }

    for(uint32_t i = 0; i < 10; ++i){
        TEST_ASSERT_TRUE(fs_mark_inode_as_free(inode[i], shm_addr) == 0);
    }

    // may change in future
    TEST_ASSERT_TRUE(fs_get_free_inode(&tmp, shm_addr) == 0);
    TEST_ASSERT_TRUE(tmp == inode[0]);
    TEST_ASSERT_TRUE(fs_mark_inode_as_used(inode[0], shm_addr) == 0);

    TEST_ASSERT_TRUE(fs_get_free_inode(&tmp, shm_addr) == 0);
    TEST_ASSERT_TRUE(tmp == inode[1]);
    TEST_ASSERT_TRUE(fs_mark_inode_as_used(inode[1], shm_addr) == 0);

    TEST_ASSERT_TRUE(fs_get_free_inode(&tmp, shm_addr) == 0);
    TEST_ASSERT_TRUE(tmp == inode[2]);

    // clean up
    for(uint32_t i = 0; i < 10; ++i){
        TEST_ASSERT_TRUE(fs_mark_inode_as_free(inode[i], shm_addr) == 0);
    }
}

int main(void){
    UNITY_BEGIN();

    setUp_inode();
    RUN_TEST(inode_setters_getters_test);
    RUN_TEST(inode_bitmap_test);
    tearDown_inode();
    
    return UNITY_END();
}
