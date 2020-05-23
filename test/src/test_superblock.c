/*
 * test_superblock.c
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
#include "block_links.h"
#include "dir_file.h"
#include "inode.h"

void* shm_addr = NULL;
const char* shm_name = "shm_test_superblock";

const uint32_t maxOpenFiles = 1024;
const uint32_t maxInodes = UINT16_MAX;
const uint32_t maxFilesystemSize = 33554432; //(32 MB);
const uint32_t sizeofOneBlock = 1024;

void setUp(void){

}

void tearDown(void){

}

void tearDown_superblock(void) {
    munmap(shm_addr, maxFilesystemSize);
    shm_unlink(shm_name);
}

void setUp_superblock(void) {
    tearDown_superblock();

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
    toSet.open_file_stat_pointer = calculate_fs_open_file_table_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock);

    toSet.inode_table_pointer = calculate_fs_open_file_stat_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock);
    toSet.inode_stat_pointer = calculate_fs_inode_table_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock);

    toSet.block_links_pointer = calculate_fs_inode_stat_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock);
    toSet.block_stat_pointer = calculate_fs_block_links_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock);
    toSet.data_blocks_pointer = calculate_fs_block_stat_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock);

    puts("Creating superblock");
    fs_create_superblock_in_shm(&toSet, shm_addr);
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
    TEST_ASSERT_EQUAL_UINT32(superblockEnd, sizeof(struct Superblock));
    TEST_ASSERT_EQUAL_UINT32(12332, openFileTableEnd);
    TEST_ASSERT_EQUAL_UINT32(12463, OpenFileStatEnd);
    TEST_ASSERT_EQUAL_UINT32(1061023, inodeTableEnd);
    TEST_ASSERT_EQUAL_UINT32(1069217, inodeStatEnd);
    TEST_ASSERT_EQUAL_UINT32(1195601, blockLinksEnd);
    TEST_ASSERT_EQUAL_UINT32(1199555, blockStatEnd);
    TEST_ASSERT_EQUAL_UINT32(33553859, dataBlockEnd);
}

void superblock_copy_test(void)
{
    struct Superblock superblockCopy;

    uint8_t ret = fs_get_superblock_copy(&superblockCopy, shm_addr);

    TEST_ASSERT_TRUE(ret == 0);

    TEST_ASSERT_EQUAL_UINT32(maxInodes, superblockCopy.max_number_of_inodes);
    TEST_ASSERT_EQUAL_UINT32(maxOpenFiles, superblockCopy.max_number_of_open_files);
    TEST_ASSERT_EQUAL_UINT16(0, superblockCopy.filesystem_checks);
    TEST_ASSERT_EQUAL_UINT32(sizeofOneBlock, superblockCopy.data_block_size);
    TEST_ASSERT_EQUAL_UINT32(calculate_fs_needed_blocks(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock), superblockCopy.number_of_data_blocks);
    TEST_ASSERT_EQUAL_UINT32(maxFilesystemSize, superblockCopy.fs_size);

    TEST_ASSERT_EQUAL_UINT32(calculate_fs_superblock_end(), superblockCopy.open_file_table_pointer);
    TEST_ASSERT_EQUAL_UINT32(calculate_fs_open_file_table_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock), superblockCopy.open_file_stat_pointer);

    TEST_ASSERT_EQUAL_UINT32(calculate_fs_open_file_stat_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock), superblockCopy.inode_table_pointer);
    TEST_ASSERT_EQUAL_UINT32(calculate_fs_inode_table_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock), superblockCopy.inode_stat_pointer);

    TEST_ASSERT_EQUAL_UINT32(calculate_fs_inode_stat_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock), superblockCopy.block_links_pointer);
    TEST_ASSERT_EQUAL_UINT32(calculate_fs_block_links_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock), superblockCopy.block_stat_pointer);
    TEST_ASSERT_EQUAL_UINT32(calculate_fs_block_stat_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock), superblockCopy.data_blocks_pointer);
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
    TEST_ASSERT_TRUE(data32 == calculate_fs_open_file_stat_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock));
    TEST_ASSERT_TRUE(fs_get_data_from_superblock_uint32(9, &data32, shm_addr) == 0);
    TEST_ASSERT_TRUE(data32 == calculate_fs_inode_table_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock));
    TEST_ASSERT_TRUE(fs_get_data_from_superblock_uint32(10, &data32, shm_addr) == 0);
    TEST_ASSERT_TRUE(data32 == calculate_fs_inode_stat_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock));
    TEST_ASSERT_TRUE(fs_get_data_from_superblock_uint32(11, &data32, shm_addr) == 0);
    TEST_ASSERT_TRUE(data32 == calculate_fs_block_links_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock));
    TEST_ASSERT_TRUE(fs_get_data_from_superblock_uint32(12, &data32, shm_addr) == 0);
    TEST_ASSERT_TRUE(data32 == calculate_fs_block_stat_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock));
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
    TEST_ASSERT_TRUE(data32 == data32Save);
    ++data32Save;

    TEST_ASSERT_TRUE(fs_save_data_to_superblock_uint32(5, data32Save, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_get_data_from_superblock_uint32(5, &data32, shm_addr) == 0);
    TEST_ASSERT_TRUE(data32 == data32Save);
    ++data32Save;

    TEST_ASSERT_TRUE(fs_save_data_to_superblock_uint32(6, data32Save, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_get_data_from_superblock_uint32(6, &data32, shm_addr) == 0);
    TEST_ASSERT_TRUE(data32 == data32Save);
    ++data32Save;

    TEST_ASSERT_TRUE(fs_save_data_to_superblock_uint32(7, data32Save, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_get_data_from_superblock_uint32(7, &data32, shm_addr) == 0);
    TEST_ASSERT_TRUE(data32 == data32Save);
    ++data32Save;

    TEST_ASSERT_TRUE(fs_save_data_to_superblock_uint32(8, data32Save, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_get_data_from_superblock_uint32(8, &data32, shm_addr) == 0);
    TEST_ASSERT_TRUE(data32 == data32Save);
    ++data32Save;

    TEST_ASSERT_TRUE(fs_save_data_to_superblock_uint32(9, data32Save, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_get_data_from_superblock_uint32(9, &data32, shm_addr) == 0);
    TEST_ASSERT_TRUE(data32 == data32Save);
    ++data32Save;

    TEST_ASSERT_TRUE(fs_save_data_to_superblock_uint32(10, data32Save, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_get_data_from_superblock_uint32(10, &data32, shm_addr) == 0);
    TEST_ASSERT_TRUE(data32 == data32Save);
    ++data32Save;

    TEST_ASSERT_TRUE(fs_save_data_to_superblock_uint32(11, data32Save, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_get_data_from_superblock_uint32(11, &data32, shm_addr) == 0);
    TEST_ASSERT_TRUE(data32 == data32Save);
    ++data32Save;

    TEST_ASSERT_TRUE(fs_save_data_to_superblock_uint32(12, data32Save, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_get_data_from_superblock_uint32(12, &data32, shm_addr) == 0);
    TEST_ASSERT_TRUE(data32 == data32Save);
    ++data32Save;
}

int main(void){
    UNITY_BEGIN();

    setUp_superblock();
    RUN_TEST(calculate_fs_offset_test);
    RUN_TEST(superblock_copy_test);
    RUN_TEST(superblock_getters_test);
    RUN_TEST(superblock_setters_test);
    tearDown_superblock();

    return UNITY_END();
}
