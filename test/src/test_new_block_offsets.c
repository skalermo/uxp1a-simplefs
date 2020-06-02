#include "unity.h"
#include "simplefs_utils.h"
#include "simplefs_api.h"
#include "memory/inode.h"
#include "memory/init.h"
#include "memory/superblock.h"
#include <semaphore.h>
#include <sys/mman.h>


void* shm_addr = NULL;

void setUp(void) {
    sem_unlink(CREATE_FS_GUARD);
    shm_unlink(FS_SHM_NAME);

    // creat file.txt
    simplefs_creat("/file.txt", 0);

    shm_addr = get_ptr_to_fs();
}

void tearDown(void) {
    unlink_fs();
}

void test_offsets(void)
{
    struct Superblock sblock;
    sblock.max_number_of_inodes = MAX_INODES;
    sblock.max_number_of_open_files = MAX_OPEN_FILES;
    sblock.filesystem_checks = 0;
    sblock.data_block_size = BLOCK_SIZE;
    sblock.fs_size = FS_SIZE;
    uint32_t total_size = 0;

    total_size += get_superblock_size();
    sblock.inode_stat_pointer = total_size;

    TEST_ASSERT_EQUAL_UINT32(44, total_size);

    total_size += get_InodeStat_size(sblock.max_number_of_inodes);
    sblock.inode_table_pointer = total_size;

    TEST_ASSERT_EQUAL_UINT32(8238, total_size);

    total_size += get_inode_table_size(sblock.max_number_of_inodes);
    sblock.open_file_stat_pointer = total_size;

    TEST_ASSERT_EQUAL_UINT32(1056798, total_size);

    total_size += get_open_file_stat_size(sblock.max_number_of_open_files);
    sblock.open_file_table_pointer = total_size;

    TEST_ASSERT_EQUAL_UINT32(1056929, total_size);

    total_size += get_open_file_table_size(sblock.max_number_of_open_files);
    sblock.block_stat_pointer = total_size;

    TEST_ASSERT_EQUAL_UINT32(1069217, total_size);

    const uint32_t size_without_blocks = total_size;
    const uint32_t block_count = get_data_block_count(FS_SIZE, size_without_blocks, sblock.data_block_size);
    sblock.number_of_data_blocks = block_count;

    total_size += get_BlockStat_size(sblock.number_of_data_blocks);
    sblock.block_links_pointer = total_size;

    TEST_ASSERT_EQUAL_UINT32(1101728, total_size);

    total_size += get_block_links_size(sblock.number_of_data_blocks);
    sblock.data_blocks_pointer = total_size;

    TEST_ASSERT_EQUAL_UINT32(2141936, total_size);


}


int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_offsets);

    return UNITY_END();
}
