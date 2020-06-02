#include "unity.h"
#include "block_links.h"
#include "superblock.h"
#include "init.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


void *shm_addr = NULL;


void setUp(void) {
    shm_addr = get_ptr_to_fs();
}


void tearDown(void) {
//    unlink_fs();
}

void dump_block(void *block_ptr) {
    FILE * fp;
    fp = fopen ("block.txt", "wb");
    fwrite(block_ptr, BLOCK_SIZE, sizeof(char), fp);
    fclose(fp);
}

void block_size() {
    struct Superblock *sblock = (struct Superblock *) shm_addr;
    TEST_ASSERT_EQUAL(BLOCK_SIZE, fs_get_data_block_size(shm_addr));
    TEST_ASSERT_EQUAL(BLOCK_SIZE, sblock->data_block_size);
}

void max_inodes() {
    struct Superblock *sblock = (struct Superblock *) shm_addr;
    TEST_ASSERT_EQUAL(MAX_INODES, fs_get_max_number_of_inodes(shm_addr));
    TEST_ASSERT_EQUAL(MAX_INODES, sblock->max_number_of_inodes);
}

void max_openfiles() {
    struct Superblock *sblock = (struct Superblock *) shm_addr;
    TEST_ASSERT_EQUAL(MAX_OPEN_FILES, fs_get_max_number_of_open_files(shm_addr));
    TEST_ASSERT_EQUAL(MAX_OPEN_FILES, sblock->max_number_of_open_files);
}

void max_datablocks() {
    struct Superblock *sblock = (struct Superblock *) shm_addr;
    TEST_ASSERT_EQUAL(sblock->number_of_data_blocks, fs_get_max_number_data_blocks(shm_addr));
}

void data_blocks_ptr() {
    struct Superblock *sblock = (struct Superblock *) shm_addr;
    void *block_ptr_calculated = shm_addr
            + get_superblock_size()
            + get_InodeStat_size(fs_get_max_number_of_inodes(shm_addr))
            + get_inode_table_size(fs_get_max_number_of_inodes(shm_addr))
            + get_open_file_stat_size(fs_get_max_number_of_open_files(shm_addr))
            + get_open_file_table_size(fs_get_max_number_of_open_files(shm_addr))
            + get_BlockStat_size(fs_get_max_number_data_blocks(shm_addr))
            + get_block_links_size(fs_get_max_number_data_blocks(shm_addr));
    TEST_ASSERT_EQUAL(block_ptr_calculated, fs_get_data_blocks_ptr(shm_addr));
}

void get_zeroth_block() {
    TEST_ASSERT_EQUAL(fs_get_data_blocks_ptr(shm_addr), get_block_ptr_by_index(0, shm_addr));
}

void get_nth_block() {
    uint32_t n = 100;
    TEST_ASSERT_EQUAL(fs_get_data_blocks_ptr(shm_addr) + n*BLOCK_SIZE, get_block_ptr_by_index(n, shm_addr));
}


int main(void) {
    UNITY_BEGIN();

    RUN_TEST(block_size);
    RUN_TEST(max_inodes);
    RUN_TEST(max_openfiles);
    RUN_TEST(max_datablocks);
    RUN_TEST(data_blocks_ptr);
    RUN_TEST(get_zeroth_block);
    RUN_TEST(get_nth_block);

    return UNITY_END();
}
