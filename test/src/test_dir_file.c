/*
 * test_dir_file.c
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
#include "block_links.h"
#include "inode.h"
#include "open_files.h"
#include "dir_file.h"

void* shm_addr = NULL;
const uint32_t sizeof_shm = 33554432; //(32 MB)
const char* shm_name = "shm_test_dir_file";

uint32_t maxOpenFiles = 1024;
uint32_t maxInodes = UINT16_MAX;
uint32_t maxFilesystemSize = sizeof_shm;
uint32_t sizeofOneBlock = 1024;

void setUp_dir_file(void){
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

    // create needed structures
    fs_create_inode_structures_in_shm(shm_addr);
    fs_create_blocks_stuctures_in_shm(shm_addr);

    fs_create_main_folder(shm_addr);
}

void tearDown_dir_file(void) {
    munmap(shm_addr, sizeof_shm);
    shm_unlink(shm_name);
}

void dir_file_only_test(void){
    uint32_t blockDirFile;
    struct FS_create_dir_data inodeInfo;
    inodeInfo.prevoiusDirInode = 0;
    inodeInfo.prevoiusDirInodeLen = 1;
    inodeInfo.prevoiusDirInodeName = 2;
    inodeInfo.thisDirInode = 3;
    inodeInfo.thisDirName = 4;
    inodeInfo.thisDirNameLen = 5;

    char testName[] = "AAC";
    char* recName = malloc(61);
    uint8_t recNameSize;
    uint16_t recInodeNumber;

    TEST_ASSERT_TRUE(fs_create_dir_file(&blockDirFile, &inodeInfo, shm_addr) == 0);

    TEST_ASSERT_TRUE(fs_save_data_to_dir_entry_name(blockDirFile, 3, testName, sizeof(testName), shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_save_data_to_dir_entry_inode_number(blockDirFile, 3, 9, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_get_data_from_dir_entry_name(blockDirFile, 3, recName, &recNameSize, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_get_data_from_dir_entry_inode_number(blockDirFile, 3, &recInodeNumber, shm_addr) == 0);
    TEST_ASSERT_TRUE(cmpmem(recName, testName, recNameSize) == 0);
    TEST_ASSERT_TRUE(recInodeNumber == 9);

    free(recName);
}

void dir_file_all_tests(void){
    setUp_dir_file();
    dir_file_only_test();
    tearDown_dir_file();
}


#endif