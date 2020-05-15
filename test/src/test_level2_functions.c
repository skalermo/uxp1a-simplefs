#include "unity.h"
#include "simplefs_utils.h"
#include "memory/inode.h"

void* shm_addr = NULL;

void setUp(void) {
    // init fs
    // mkdir dir
    // creat file.txt
}

void tearDown(void) {
}

void test_get_inode_index(void)
{
    int idx = get_inode_index("/dir/file.txt", shm_addr);

    TEST_ASSERT_GREATER_THAN(0, idx);
}

void test_get_inode(void)
{
    int idx = get_inode_index("/dir/file.txt", shm_addr);
    TEST_ASSERT_GREATER_THAN(0, idx);


    int block_idx = get_inode_block_index(idx, shm_addr);
    TEST_ASSERT_GREATER_THAN(-1, idx);

    int file_size = get_inode_file_size(idx, shm_addr);
    TEST_ASSERT_EQUAL(0, file_size);

    short readers = get_inode_readers(idx,shm_addr);
    TEST_ASSERT_EQUAL(0, readers);

    short writers = get_inode_writers(idx,shm_addr);
    TEST_ASSERT_EQUAL(0, writers);

    char mode = get_inode_mode(idx,shm_addr);
    TEST_ASSERT_EQUAL(0, mode);

    char ref_count =  get_ref_count(idx,shm_addr);
    TEST_ASSERT_EQUAL(0, ref_count);

    struct Inode inode = get_inode(idx, shm_addr);
    TEST_ASSERT_EQUAL(block_idx, inode.block_index);
    TEST_ASSERT_EQUAL(file_size, inode.file_size);
    TEST_ASSERT_EQUAL(readers, inode.readers);
    TEST_ASSERT_EQUAL(writers, inode.writers);
    TEST_ASSERT_EQUAL(mode, inode.mode);
    TEST_ASSERT_EQUAL(ref_count, inode.ref_count);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_get_inode_index);
    RUN_TEST(test_get_inode);

    return UNITY_END();
}
