/*
 * test_open_files.c
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
const char* shm_name = "shm_test_open_files";

const uint32_t maxOpenFiles = 1024;
const uint32_t maxInodes = UINT16_MAX;
const uint32_t maxFilesystemSize = 33554432; //(32 MB);
const uint32_t sizeofOneBlock = 1024;

void setUp(void){

}

void tearDown(void){

}

void tearDown_open_files(void) {
    munmap(shm_addr, maxFilesystemSize);
    shm_unlink(shm_name);
}

void setUp_open_files(void){
    tearDown_open_files();

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

    fs_create_superblock_in_shm(&toSet, shm_addr);

    // create open files structures
    fs_create_open_file_table_stuctures_in_shm(shm_addr);
}

void open_files_getters_test(void){
    uint16_t data16;
    uint32_t data32;
    uint32_t openFile0, openFile1, openFile2, openFile3, openFile4, openFile5, openFile6, openFile7, openFile8, openFile9;
    struct OpenFile toSave;
    toSave.inode_num = 25;
    toSave.mode = 2;
    toSave.offset = 67;

    // adds opened files
    TEST_ASSERT_TRUE(fs_occupy_free_open_file(&openFile0, &toSave, shm_addr) == 0);

    ++toSave.inode_num;
    ++toSave.mode;
    ++toSave.offset;
    TEST_ASSERT_TRUE(fs_occupy_free_open_file(&openFile1, &toSave, shm_addr) == 0);

    ++toSave.inode_num;
    ++toSave.mode;
    ++toSave.offset;
    TEST_ASSERT_TRUE(fs_occupy_free_open_file(&openFile2, &toSave, shm_addr) == 0);

    ++toSave.inode_num;
    ++toSave.mode;
    ++toSave.offset;
    TEST_ASSERT_TRUE(fs_occupy_free_open_file(&openFile3, &toSave, shm_addr) == 0);

    ++toSave.inode_num;
    ++toSave.mode;
    ++toSave.offset;
    TEST_ASSERT_TRUE(fs_occupy_free_open_file(&openFile4, &toSave, shm_addr) == 0);

    ++toSave.inode_num;
    ++toSave.mode;
    ++toSave.offset;
    TEST_ASSERT_TRUE(fs_occupy_free_open_file(&openFile5, &toSave, shm_addr) == 0);

    ++toSave.inode_num;
    ++toSave.mode;
    ++toSave.offset;
    TEST_ASSERT_TRUE(fs_occupy_free_open_file(&openFile6, &toSave, shm_addr) == 0);
    
    ++toSave.inode_num;
    ++toSave.mode;
    ++toSave.offset;
    TEST_ASSERT_TRUE(fs_occupy_free_open_file(&openFile7, &toSave, shm_addr) == 0);

    ++toSave.inode_num;
    ++toSave.mode;
    ++toSave.offset;
    TEST_ASSERT_TRUE(fs_occupy_free_open_file(&openFile8, &toSave, shm_addr) == 0);

    ++toSave.inode_num;
    ++toSave.mode;
    ++toSave.offset;
    TEST_ASSERT_TRUE(fs_occupy_free_open_file(&openFile9, &toSave, shm_addr) == 0);


    // test getters
    TEST_ASSERT_TRUE(fs_get_data_from_open_file_uint16(openFile0, 0, &data16, shm_addr) == 0);
    TEST_ASSERT_TRUE(data16 == 2);
    TEST_ASSERT_TRUE(fs_get_data_from_open_file_uint16(openFile5, 1, &data16, shm_addr) == 0);
    TEST_ASSERT_TRUE(data16 == 30);
    TEST_ASSERT_TRUE(fs_get_data_from_open_file_uint16(openFile3, 1, &data16, shm_addr) == 0);
    TEST_ASSERT_TRUE(data16 == 28);
    TEST_ASSERT_TRUE(fs_get_data_from_open_file_uint16(openFile7, 1, &data16, shm_addr) == 0);
    TEST_ASSERT_TRUE(data16 == 32);
    TEST_ASSERT_TRUE(fs_get_data_from_open_file_uint16(openFile6, 0, &data16, shm_addr) == 0);
    TEST_ASSERT_TRUE(data16 == 8);

    TEST_ASSERT_TRUE(fs_get_data_from_open_file_uint32(openFile6, 2, &data32, shm_addr) == 0);
    TEST_ASSERT_TRUE(data32 == 73);
    TEST_ASSERT_TRUE(fs_get_data_from_open_file_uint32(openFile9, 2, &data32, shm_addr) == 0);
    TEST_ASSERT_TRUE(data32 == 76);
    TEST_ASSERT_TRUE(fs_get_data_from_open_file_uint32(openFile8, 2, &data32, shm_addr) == 0);
    TEST_ASSERT_TRUE(data32 == 75);
    TEST_ASSERT_TRUE(fs_get_data_from_open_file_uint32(openFile1, 2, &data32, shm_addr) == 0);
    TEST_ASSERT_TRUE(data32 == 68);
    TEST_ASSERT_TRUE(fs_get_data_from_open_file_uint32(openFile6, 2, &data32, shm_addr) == 0);
    TEST_ASSERT_TRUE(data32 == 73);

    uint16_t index, index2;
    TEST_ASSERT_TRUE(fs_get_free_open_file(&index, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_get_free_open_file(&index2, shm_addr) == 0);
    TEST_ASSERT_TRUE(index == index2); // this may change too as below described
    TEST_ASSERT_TRUE(index != openFile0 && index != openFile1 && index != openFile2 && index != openFile3 && index != openFile4);
    TEST_ASSERT_TRUE(index != openFile5 && index != openFile6 && index != openFile7 && index != openFile8 && index != openFile9);

    TEST_ASSERT_TRUE(fs_mark_open_file_as_free(openFile3, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_mark_open_file_as_free(openFile8, shm_addr) == 0);

    TEST_ASSERT_TRUE(fs_get_free_open_file(&index, shm_addr) == 0);

    // later this may change output if it changes how free open files are selected.
    TEST_ASSERT_TRUE(fs_get_free_open_file(&index, shm_addr) == 0);
    TEST_ASSERT_TRUE(index == openFile3);
    TEST_ASSERT_TRUE(fs_get_free_open_file(&index, shm_addr) == 0);
    TEST_ASSERT_TRUE(index == openFile3);

    TEST_ASSERT_TRUE(fs_mark_open_file_as_used(openFile3, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_mark_open_file_as_used(openFile8, shm_addr) == 0);
}

void open_files_setters_test(void){
    uint32_t openFile0, openFile1, openFile2, openFile3, openFile4, openFile5, openFile6, openFile7, openFile8, openFile9;

    struct OpenFile toSave;
    uint16_t inode_num = 64;
    uint16_t mode = 752;
    uint32_t offset = 126;
    toSave.inode_num = inode_num;
    toSave.mode = mode;
    toSave.offset = offset;


    // adds opened files
    TEST_ASSERT_TRUE(fs_occupy_free_open_file(&openFile0, &toSave, shm_addr) == 0);

    ++toSave.inode_num;
    ++toSave.mode;
    ++toSave.offset;
    TEST_ASSERT_TRUE(fs_occupy_free_open_file(&openFile1, &toSave, shm_addr) == 0);

    ++toSave.inode_num;
    ++toSave.mode;
    ++toSave.offset;
    TEST_ASSERT_TRUE(fs_occupy_free_open_file(&openFile2, &toSave, shm_addr) == 0);

    ++toSave.inode_num;
    ++toSave.mode;
    ++toSave.offset;
    TEST_ASSERT_TRUE(fs_occupy_free_open_file(&openFile3, &toSave, shm_addr) == 0);

    ++toSave.inode_num;
    ++toSave.mode;
    ++toSave.offset;
    TEST_ASSERT_TRUE(fs_occupy_free_open_file(&openFile4, &toSave, shm_addr) == 0);

    ++toSave.inode_num;
    ++toSave.mode;
    ++toSave.offset;
    TEST_ASSERT_TRUE(fs_occupy_free_open_file(&openFile5, &toSave, shm_addr) == 0);

    ++toSave.inode_num;
    ++toSave.mode;
    ++toSave.offset;
    TEST_ASSERT_TRUE(fs_occupy_free_open_file(&openFile6, &toSave, shm_addr) == 0);
    
    ++toSave.inode_num;
    ++toSave.mode;
    ++toSave.offset;
    TEST_ASSERT_TRUE(fs_occupy_free_open_file(&openFile7, &toSave, shm_addr) == 0);

    ++toSave.inode_num;
    ++toSave.mode;
    ++toSave.offset;
    TEST_ASSERT_TRUE(fs_occupy_free_open_file(&openFile8, &toSave, shm_addr) == 0);

    ++toSave.inode_num;
    ++toSave.mode;
    ++toSave.offset;
    TEST_ASSERT_TRUE(fs_occupy_free_open_file(&openFile9, &toSave, shm_addr) == 0);

    // setters
    uint16_t data16 = 999;
    uint32_t data32 = 15642;
    uint16_t data16R;
    uint32_t data32R;

    TEST_ASSERT_TRUE(fs_save_data_to_open_file_uint16(openFile7, 1, data16, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_get_data_from_open_file_uint16(openFile7, 1, &data16R, shm_addr) == 0);
    TEST_ASSERT_TRUE(data16R == data16);

    TEST_ASSERT_TRUE(fs_save_data_to_open_file_uint16(openFile2, 1, data16, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_get_data_from_open_file_uint16(openFile2, 1, &data16R, shm_addr) == 0);
    TEST_ASSERT_TRUE(data16R == data16);

    TEST_ASSERT_TRUE(fs_save_data_to_open_file_uint16(openFile9, 0, data16, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_get_data_from_open_file_uint16(openFile9, 0, &data16R, shm_addr) == 0);
    TEST_ASSERT_TRUE(data16R == data16);

    TEST_ASSERT_TRUE(fs_save_data_to_open_file_uint16(openFile8, 1, data16, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_get_data_from_open_file_uint16(openFile8, 1, &data16R, shm_addr) == 0);
    TEST_ASSERT_TRUE(data16R == data16);

    TEST_ASSERT_TRUE(fs_save_data_to_open_file_uint16(openFile7, 0, data16, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_get_data_from_open_file_uint16(openFile7, 0, &data16R, shm_addr) == 0);
    TEST_ASSERT_TRUE(data16R == data16);

    // 32 bit
    TEST_ASSERT_TRUE(fs_save_data_to_open_file_uint32(openFile9, 2, data32, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_get_data_from_open_file_uint32(openFile9, 2, &data32R, shm_addr) == 0);
    TEST_ASSERT_TRUE(data16R == data16);

    TEST_ASSERT_TRUE(fs_save_data_to_open_file_uint32(openFile1, 2, data32, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_get_data_from_open_file_uint32(openFile1, 2, &data32R, shm_addr) == 0);
    TEST_ASSERT_TRUE(data16R == data16);
    
    TEST_ASSERT_TRUE(fs_save_data_to_open_file_uint32(openFile3, 2, data32, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_get_data_from_open_file_uint32(openFile3, 2, &data32R, shm_addr) == 0);
    TEST_ASSERT_TRUE(data16R == data16);
}

void open_files_bitmap_test(void){
    uint32_t openFile0, openFile1, openFile2, openFile3, openFile4, openFile5, openFile6, openFile7, openFile8, openFile9;
    struct OpenFile toSave;
    uint16_t inode_num = 805;
    uint16_t mode = 1024;
    uint32_t offset = 55556;
    toSave.inode_num = inode_num;
    toSave.mode = mode;
    toSave.offset = offset;


    // adds opened files
    TEST_ASSERT_TRUE(fs_occupy_free_open_file(&openFile0, &toSave, shm_addr) == 0);

    ++toSave.inode_num;
    ++toSave.mode;
    ++toSave.offset;
    TEST_ASSERT_TRUE(fs_occupy_free_open_file(&openFile1, &toSave, shm_addr) == 0);

    ++toSave.inode_num;
    ++toSave.mode;
    ++toSave.offset;
    TEST_ASSERT_TRUE(fs_occupy_free_open_file(&openFile2, &toSave, shm_addr) == 0);

    ++toSave.inode_num;
    ++toSave.mode;
    ++toSave.offset;
    TEST_ASSERT_TRUE(fs_occupy_free_open_file(&openFile3, &toSave, shm_addr) == 0);

    ++toSave.inode_num;
    ++toSave.mode;
    ++toSave.offset;
    TEST_ASSERT_TRUE(fs_occupy_free_open_file(&openFile4, &toSave, shm_addr) == 0);

    ++toSave.inode_num;
    ++toSave.mode;
    ++toSave.offset;
    TEST_ASSERT_TRUE(fs_occupy_free_open_file(&openFile5, &toSave, shm_addr) == 0);

    ++toSave.inode_num;
    ++toSave.mode;
    ++toSave.offset;
    TEST_ASSERT_TRUE(fs_occupy_free_open_file(&openFile6, &toSave, shm_addr) == 0);
    
    ++toSave.inode_num;
    ++toSave.mode;
    ++toSave.offset;
    TEST_ASSERT_TRUE(fs_occupy_free_open_file(&openFile7, &toSave, shm_addr) == 0);

    ++toSave.inode_num;
    ++toSave.mode;
    ++toSave.offset;
    TEST_ASSERT_TRUE(fs_occupy_free_open_file(&openFile8, &toSave, shm_addr) == 0);

    ++toSave.inode_num;
    ++toSave.mode;
    ++toSave.offset;
    TEST_ASSERT_TRUE(fs_occupy_free_open_file(&openFile9, &toSave, shm_addr) == 0);

    // bitmap

    uint16_t freeIndex;

    TEST_ASSERT_TRUE(fs_mark_open_file_as_free(openFile6, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_mark_open_file_as_free(openFile2, shm_addr) == 0);
    TEST_ASSERT_TRUE(fs_mark_open_file_as_free(openFile8, shm_addr) == 0);

    TEST_ASSERT_TRUE(fs_get_free_open_file(&freeIndex, shm_addr) == 0);
    TEST_ASSERT_TRUE(freeIndex == openFile2);
    TEST_ASSERT_TRUE(fs_mark_open_file_as_used(freeIndex, shm_addr) == 0);

    TEST_ASSERT_TRUE(fs_get_free_open_file(&freeIndex, shm_addr) == 0);
    TEST_ASSERT_TRUE(freeIndex == openFile6);
    TEST_ASSERT_TRUE(fs_mark_open_file_as_used(freeIndex, shm_addr) == 0);

    TEST_ASSERT_TRUE(fs_get_free_open_file(&freeIndex, shm_addr) == 0);
    TEST_ASSERT_TRUE(freeIndex == openFile8);
    TEST_ASSERT_TRUE(fs_mark_open_file_as_used(freeIndex, shm_addr) == 0);

    TEST_ASSERT_TRUE(fs_get_free_open_file(&freeIndex, shm_addr) == 0);
    TEST_ASSERT_TRUE(freeIndex != openFile6 && freeIndex != openFile2 && freeIndex != openFile8);
    TEST_ASSERT_TRUE(fs_mark_open_file_as_used(freeIndex, shm_addr) == 0);

}

int main(void){
    UNITY_BEGIN();

    setUp_open_files();
    RUN_TEST(open_files_getters_test);
    RUN_TEST(open_files_setters_test);
    RUN_TEST(open_files_bitmap_test);
    tearDown_open_files();
    
    return UNITY_END();
}
