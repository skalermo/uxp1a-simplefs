/*
 * test_superblock.c
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
#include "block_links.h"
#include "dir_file.h"
#include "inode.h"

void* shm_addr = NULL;
const uint32_t sizeof_shm = 33554432; //(32 MB)
const char* shm_name = "shm_test_superblock";

uint32_t maxOpenFiles = 1024;
uint32_t maxInodes = UINT16_MAX;
uint32_t maxFilesystemSize = sizeof_shm;
uint32_t sizeofOneBlock = 1024;

void setUp_superblock(void) {
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
}

void tearDown_superblock(void) {
    munmap(shm_addr, sizeof_shm);
    shm_unlink(shm_name);
}

void calculate_fs_offset_test(void){

    uint32_t superblockEnd = calculate_fs_superblock_end();
    uint32_t openFileTableEnd = calculate_fs_open_file_table_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock);
    uint32_t OpenFileStatEnd = calculate_fs_open_file_stat_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock);
    uint32_t inodeTableEnd = calculate_fs_inode_table_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock);
    uint32_t inodeStatEnd = calculate_fs_inode_stat_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock);
    uint32_t blockLinksEnd = calculate_fs_block_links_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock);
    uint32_t blockStatEnd = calculate_fs_block_stat_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock);
    uint32_t dataBlockEnd = calculate_fs_data_block_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock);

    // to calculate this, use excel
    TEST_ASSERT_TRUE(superblockEnd == sizeof(struct Superblock));
    TEST_ASSERT_TRUE(openFileTableEnd == 8232);
    TEST_ASSERT_TRUE(OpenFileStatEnd == 8364);
    TEST_ASSERT_TRUE(inodeTableEnd == 794784);
    TEST_ASSERT_TRUE(inodeStatEnd == 802978);
    TEST_ASSERT_TRUE(blockLinksEnd == 930398);
    TEST_ASSERT_TRUE(blockStatEnd == 934380);
    TEST_ASSERT_TRUE(dataBlockEnd == 33553900);
}

void superblock_copy_test(void)
{
    struct Superblock superblockCopy;

    uint8_t ret = fs_get_superblock_copy(&superblockCopy, shm_addr);
    
    TEST_ASSERT_TRUE(ret == 0);

    TEST_ASSERT_TRUE(superblockCopy.max_number_of_inodes == maxInodes);
    TEST_ASSERT_TRUE(superblockCopy.max_number_of_open_files == maxOpenFiles);
    TEST_ASSERT_TRUE(superblockCopy.filesystem_checks == 0);
    TEST_ASSERT_TRUE(superblockCopy.data_block_size == sizeofOneBlock);
    TEST_ASSERT_TRUE(superblockCopy.number_of_data_blocks == calculate_fs_needed_blocks(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock));
    TEST_ASSERT_TRUE(superblockCopy.fs_size == maxFilesystemSize);

    TEST_ASSERT_TRUE(superblockCopy.open_file_table_pointer == calculate_fs_superblock_end());
    TEST_ASSERT_TRUE(superblockCopy.open_file_bitmap_pointer == calculate_fs_open_file_table_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock));

    TEST_ASSERT_TRUE(superblockCopy.inode_table_pointer == calculate_fs_open_file_stat_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock));
    TEST_ASSERT_TRUE(superblockCopy.inode_bitmap_pointer == calculate_fs_inode_table_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock));

    TEST_ASSERT_TRUE(superblockCopy.block_links_pointer == calculate_fs_inode_stat_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock));
    TEST_ASSERT_TRUE(superblockCopy.block_bitmap_pointer) == calculate_fs_block_links_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock);
    TEST_ASSERT_TRUE(superblockCopy.data_blocks_pointer == calculate_fs_block_stat_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock));
}

void superblock_getters_test(void){
    uint16_t data16;
    uint32_t data32;

    TEST_ASSERT_TRUE(fs_get_data_block_size(shm_addr) == sizeofOneBlock);
    TEST_ASSERT_TRUE(fs_get_max_number_of_inodes(shm_addr) == maxInodes);

    TEST_ASSERT_TRUE(fs_get_data_from_superblock_uint16(0, &data16, shm_addr) == 0);
    TEST_ASSERT_TRUE(data16 == maxInodes);
    TEST_ASSERT_TRUE(fs_get_data_from_superblock_uint16(1, &data16, shm_addr) == 0);
    TEST_ASSERT_TRUE(data16 == maxOpenFiles);
    TEST_ASSERT_TRUE(fs_get_data_from_superblock_uint16(2, &data16, shm_addr) == 0);
    TEST_ASSERT_TRUE(data16 == 0);
    TEST_ASSERT_TRUE(fs_get_data_from_superblock_uint16(3, &data16, shm_addr) == 0);
    TEST_ASSERT_TRUE(data16 == sizeofOneBlock);

    TEST_ASSERT_TRUE(fs_get_data_from_superblock_uint32(4, &data32, shm_addr) == 0);
    TEST_ASSERT_TRUE(data32 == calculate_fs_needed_blocks(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock));
    TEST_ASSERT_TRUE(fs_get_data_from_superblock_uint32(5, &data32, shm_addr) == 0);
    TEST_ASSERT_TRUE(data32 == maxFilesystemSize);
    TEST_ASSERT_TRUE(fs_get_data_from_superblock_uint32(6, &data32, shm_addr) == 0);
    TEST_ASSERT_TRUE(data32 == calculate_fs_superblock_end());
    TEST_ASSERT_TRUE(fs_get_data_from_superblock_uint32(7, &data32, shm_addr) == 0);
    TEST_ASSERT_TRUE(data32 == calculate_fs_open_file_table_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock));
    TEST_ASSERT_TRUE(fs_get_data_from_superblock_uint32(8, &data32, shm_addr) == 0);
    TEST_ASSERT_TRUE(data32 == calculate_fs_inode_stat_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock));
    TEST_ASSERT_TRUE(fs_get_data_from_superblock_uint32(9, &data32, shm_addr) == 0);
    TEST_ASSERT_TRUE(data32 == calculate_fs_block_links_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock));
    TEST_ASSERT_TRUE(fs_get_data_from_superblock_uint32(10, &data32, shm_addr) == 0);
    TEST_ASSERT_TRUE(data32 == calculate_fs_block_stat_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock));
    TEST_ASSERT_TRUE(fs_get_data_from_superblock_uint32(11, &data32, shm_addr) == 0);
    TEST_ASSERT_TRUE(data32 == calculate_fs_open_file_stat_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock));
    TEST_ASSERT_TRUE(fs_get_data_from_superblock_uint32(12, &data32, shm_addr) == 0);
    TEST_ASSERT_TRUE(TEST_ASSERT_TRUE(data32 == calculate_fs_inode_table_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock)));
}

/**
 * @brief It corrupts the superblock structure.
 * This test should be performed as last.
 * 
 */
void superblock_setters_test(void){
    uint16_t data16Save = 23;
    uint32_t data32Save = 61;
    uint16_t data16;
    uint32_t data32;

    TEST_ASSERT_TRUE(fs_save_data_to_superblock_uint16(0, data16Save, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_get_data_from_superblock_uint16(0, &data16, shm_addr) == 0);
    TEST_ASSERT_TRUE(data16 == data16Save);
    ++data16Save;

    TEST_ASSERT_TRUE(fs_save_data_to_superblock_uint16(1, data16Save, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_get_data_from_superblock_uint16(1, &data16, shm_addr) == 0);
    TEST_ASSERT_TRUE(data16 == data16Save);
    ++data16Save;

    TEST_ASSERT_TRUE(fs_save_data_to_superblock_uint16(2, data16Save, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_get_data_from_superblock_uint16(2, &data16, shm_addr) == 0);
    TEST_ASSERT_TRUE(data16 == data16Save);
    ++data16Save;

    TEST_ASSERT_TRUE(fs_save_data_to_superblock_uint16(3, data16Save, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_get_data_from_superblock_uint16(3, &data16, shm_addr) == 0);
    TEST_ASSERT_TRUE(data16 == data16Save);
    ++data16Save;

    TEST_ASSERT_TRUE(fs_save_data_to_superblock_uint32(4, data32Save, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_get_data_from_superblock_uint32(4, &data32, shm_addr) == 0);
    TEST_ASSERT_TRUE(data32 == maxFilesystemSize);
    ++data32Save;

    TEST_ASSERT_TRUE(fs_save_data_to_superblock_uint32(5, data32Save, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_get_data_from_superblock_uint32(5, &data32, shm_addr) == 0);
    TEST_ASSERT_TRUE(data32 == maxFilesystemSize);
    ++data32Save;

    TEST_ASSERT_TRUE(fs_save_data_to_superblock_uint32(6, data32Save, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_get_data_from_superblock_uint32(6, &data32, shm_addr) == 0);
    TEST_ASSERT_TRUE(data32 == calculate_fs_superblock_end());
    ++data32Save;

    TEST_ASSERT_TRUE(fs_save_data_to_superblock_uint32(7, data32Save, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_get_data_from_superblock_uint32(7, &data32, shm_addr) == 0);
    TEST_ASSERT_TRUE(data32 == calculate_fs_open_file_table_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock));
    ++data32Save;

    TEST_ASSERT_TRUE(fs_save_data_to_superblock_uint32(8, data32Save, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_get_data_from_superblock_uint32(8, &data32, shm_addr) == 0);
    TEST_ASSERT_TRUE(data32 == calculate_fs_inode_stat_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock));
    ++data32Save;

    TEST_ASSERT_TRUE(fs_save_data_to_superblock_uint32(9, data32Save, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_get_data_from_superblock_uint32(9, &data32, shm_addr) == 0);
    TEST_ASSERT_TRUE(data32 == calculate_fs_block_links_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock));
    ++data32Save;

    TEST_ASSERT_TRUE(fs_save_data_to_superblock_uint32(10, data32Save, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_get_data_from_superblock_uint32(10, &data32, shm_addr) == 0);
    TEST_ASSERT_TRUE(data32 == calculate_fs_block_stat_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock));
    ++data32Save;

    TEST_ASSERT_TRUE(fs_save_data_to_superblock_uint32(11, data32Save, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_get_data_from_superblock_uint32(11, &data32, shm_addr) == 0);
    TEST_ASSERT_TRUE(data32 == calculate_fs_open_file_stat_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock));
    ++data32Save;

    TEST_ASSERT_TRUE(fs_save_data_to_superblock_uint32(12, data32Save, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_get_data_from_superblock_uint32(12, &data32, shm_addr) == 0);
    TEST_ASSERT_TRUE(TEST_ASSERT_TRUE(data32 == calculate_fs_inode_table_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock)));
    ++data32Save;
}

void setUp_superblock(void){
    setUp_superblock();
    calculate_fs_offset_test();
    superblock_copy_test();
    superblock_getters_test();
    superblock_setters_test();
    tearDown_superblock();
}

#endif