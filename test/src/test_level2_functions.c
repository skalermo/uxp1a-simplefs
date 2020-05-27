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

void test_get_inode_index(void)
{
    int idx = get_inode_index("/file.txt", IS_FILE, shm_addr);

    TEST_ASSERT_GREATER_THAN(0, idx);
}

void test_get_inode(void)
{
    int idx = get_inode_index("/file.txt", IS_FILE, shm_addr);
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
