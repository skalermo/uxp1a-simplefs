/*
 * test_inode.c
 *
 *      Author: Kordowski Mateusz
 */


#ifndef SIMPLEFS_SUPERBLOCK_TEST_C
#define SIMPLEFS_SUPERBLOCK_TEST_C

#include <sys/mman.h>
#include <sys/stat.h> 
#include <fcntl.h> 

#include "unity.h"
#include "superblock.h"
#include "open_files.h"

void* shm_addr = NULL;
const uint32_t sizeof_shm = 33554432; //(32 MB)
const char* shm_name = "shm_test_inodes";

uint32_t maxOpenFiles = 1024;
uint32_t maxInodes = UINT16_MAX;
uint32_t maxFilesystemSize = sizeof_shm;
uint32_t sizeofOneBlock = 1024;

void setUp_inode(void){
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
    fs_create_inode_structures_in_shm(shm_addr);
}

void tearDown_inode(void) {
    munmap(shm_addr, sizeof_shm);
    shm_unlink(shm_name);
}

void inode_setters_getters_test(void){
    uint16_t inodes[10];
    uint32_t data32 = 565411;
    uint16_t data16 = 4864;
    uint8_t data8 = 26;

    uint8_t offset1 = 11;
    uint8_t offset2 = 22;
    uint8_t offset3 = 33;
    
    uint32_t data32S = data32;
    uint16_t data16S = data16;
    uint8_t data8S1 = data8 + offset1;
    uint8_t data8S2 = data8 + offset2;
    uint8_t data8S3 = data8 + offset3;

    uint32_t data32R;
    uint16_t data16R;
    uint8_t data8R;

    for(uint32_t i = 0; i < 10; ++i){
        TEST_ASSERT_TRUE(fs_get_free_inode(&inodes[i], shm_addr) == 0);
    }

    for(uint32_t i = 0; i < 10; ++i){
        TEST_ASSERT_TRUE(fs_save_data_to_inode_uint32(inodes[i], 0, data32S, shm_addr) == 0);
        ++data32S;
    }
    
    for(uint32_t i = 0; i < 10; ++i){
        TEST_ASSERT_TRUE(fs_save_data_to_inode_uint16(inodes[i], 1, data16S, shm_addr) == 0);
        ++data16S;
    }

    for(uint32_t i = 0; i < 10; ++i){
        TEST_ASSERT_TRUE(fs_save_data_to_inode_uint16(inodes[i], 2, data8S1, shm_addr) == 0);
        ++data8S1;
        TEST_ASSERT_TRUE(fs_save_data_to_inode_uint16(inodes[i], 3, data8S2, shm_addr) == 0);
        ++data8S2;
        TEST_ASSERT_TRUE(fs_save_data_to_inode_uint16(inodes[i], 4, data8S3, shm_addr) == 0);
        ++data8S3;
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
        TEST_ASSERT_TRUE(fs_get_data_from_inode_uint8(inodes[i], 2, &data8R, shm_addr) == 0);
        TEST_ASSERT_TRUE(data8R == data8 + i + offset1);

        TEST_ASSERT_TRUE(fs_get_data_from_inode_uint8(inodes[i], 3, &data8R, shm_addr) == 0);
        TEST_ASSERT_TRUE(data8R == data8 + i + offset2);

        TEST_ASSERT_TRUE(fs_get_data_from_inode_uint8(inodes[i], 4, &data8R, shm_addr) == 0);
        TEST_ASSERT_TRUE(data8R == data8 + i + offset3);
    }

    struct Inode inod;

    for(uint32_t i = 0; i < 10; ++i){
        TEST_ASSERT_TRUE(fs_get_inode_copy(inodes[i], &inod, shm_addr) == 0);
        TEST_ASSERT_TRUE(inod.block_index == data32 + i);
        TEST_ASSERT_TRUE(inod.file_size == data16 + i);
        TEST_ASSERT_TRUE(inod.mode == data8 + i + offset1);
        TEST_ASSERT_TRUE(inod.ref_count == data8 + i + offset2);
        TEST_ASSERT_TRUE(inod.readers == data8 + i + offset3);
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

    uint32_t inode[10];
    uint32_t tmp;

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

void inode_all_tests(void){
    setUp_inode();
    inode_setters_getters_test();
    inode_bitmap_test();
    tearDown_inode();
}

#endif