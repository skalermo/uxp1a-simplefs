/*
 * test_dir_file.c
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
#include "memory/superblock.h"
#include "memory/block_links.h"
#include "memory/inode.h"
#include "memory/open_files.h"
#include "memory/dir_file.h"

void* shm_addr = NULL;
const char* shm_name = "shm_test_dir_file";

const uint32_t maxOpenFiles = 1024;
const uint32_t maxInodes = UINT16_MAX;
const uint32_t maxFilesystemSize = 33554432; //(32 MB);
const uint32_t sizeofOneBlock = 1024;

void setUp(void){

}

void tearDown(void){

}

void tearDown_dir_file(void) {
    munmap(shm_addr, maxFilesystemSize);
    shm_unlink(shm_name);
}

void setUp_dir_file(void){
    tearDown_dir_file();

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

    // create needed structures
    fs_create_inode_structures_in_shm(shm_addr);
    fs_create_blocks_stuctures_in_shm(shm_addr);

    fs_create_main_folder(shm_addr);
}

void dir_file_only_test(void){
    uint32_t blockDirFile;
    struct FS_create_dir_data inodeInfo;
    char* name1 = "";
    char* name2 = "DDDD";

    inodeInfo.prevoiusDirInode = 0;
    inodeInfo.prevoiusDirInodeLen = 1;
    inodeInfo.prevoiusDirInodeName = malloc(61);
    memcpy(inodeInfo.prevoiusDirInodeName, name1, 1);
    inodeInfo.thisDirInode = 3;
    inodeInfo.thisDirName = malloc(61);
    memcpy(inodeInfo.thisDirName, name2, 5);
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
    TEST_ASSERT_TRUE(memcmp(recName, testName, recNameSize) == 0);
    TEST_ASSERT_TRUE(recInodeNumber == 9);

    free(recName);
    free(inodeInfo.prevoiusDirInodeName);
    free(inodeInfo.thisDirName);
}

int main(void){
    UNITY_BEGIN();

    setUp_dir_file();
    RUN_TEST(dir_file_only_test);
    tearDown_dir_file();

    return UNITY_END();
}

