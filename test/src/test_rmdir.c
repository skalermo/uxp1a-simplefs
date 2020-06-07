#include "unity.h"
#include "simplefs_utils.h"
#include "init.h"
#include <stdlib.h>
#include <unistd.h>


void *shm_addr = NULL;


void setUp(void) {
    shm_addr = get_ptr_to_fs();
}


void tearDown(void) {
    unlink_fs();
}

void remove_root_or_1char_dir() {
    int ret_value = simplefs_rmdir("/");
    TEST_ASSERT_EQUAL(ENOENT, ret_value);

    ret_value = simplefs_rmdir("a");
    TEST_ASSERT_EQUAL(ENOENT, ret_value);

    ret_value = simplefs_rmdir("b");
    TEST_ASSERT_EQUAL(ENOENT, ret_value);
}

void remove_not_root_subdir() {
    int ret_value = simplefs_rmdir("somedir");
    TEST_ASSERT_EQUAL(ENOENT, ret_value);
}

void too_long_filename() {
    char *too_long_name = "/"
                          "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                          "aaaaaaaaaaab";
    int ret_value = simplefs_rmdir(too_long_name);
    TEST_ASSERT_EQUAL(ENAMETOOLONG, ret_value);
}

void remove_non_existent_file() {
    int ret_value = simplefs_rmdir("/file_that_doesnt_exist");
    TEST_ASSERT_EQUAL(ENOENT, ret_value);
}

void remove_not_empty_dir() {
    int ret_value = simplefs_mkdir("/a");
    TEST_ASSERT_EQUAL(0, ret_value);
    ret_value = simplefs_mkdir("/a/b");
    TEST_ASSERT_EQUAL(0, ret_value);

    ret_value = simplefs_rmdir("/a");
    TEST_ASSERT_EQUAL(ENOTEMPTY, ret_value);
}

void remove_one_dir() {
    // test stuff before any operation on fs
    uint16_t inode_count = fs_get_used_inodes(shm_addr);
    TEST_ASSERT_EQUAL(2, inode_count);
    uint32_t block_count = fs_get_used_blocks(shm_addr);
    TEST_ASSERT_EQUAL(1, block_count);
    int bit_set = is_bit_set(2, fs_get_inode_bitmap_ptr(shm_addr));
    TEST_ASSERT_EQUAL(0, bit_set);
    bit_set = is_bit_set(1, fs_get_block_bitmap_ptr(shm_addr));
    TEST_ASSERT_EQUAL(0, bit_set);

    // create single dir
    int ret_value = simplefs_mkdir("/a");
    TEST_ASSERT_EQUAL(0, ret_value);

    // test stuff after creating dir "/a"
    inode_count = fs_get_used_inodes(shm_addr);
    TEST_ASSERT_EQUAL(3, inode_count);
    block_count = fs_get_used_blocks(shm_addr);
    TEST_ASSERT_EQUAL(2, block_count);
    bit_set = is_bit_set(2, fs_get_inode_bitmap_ptr(shm_addr));
    TEST_ASSERT_EQUAL(1, bit_set);
    bit_set = is_bit_set(1, fs_get_block_bitmap_ptr(shm_addr));
    TEST_ASSERT_EQUAL(1, bit_set);

    // remove dir "/a"
    ret_value = simplefs_rmdir("/a");
    TEST_ASSERT_EQUAL(0, ret_value);

    // test stuff after removing dir "/a"
    inode_count = fs_get_used_inodes(shm_addr);
    TEST_ASSERT_EQUAL(2, inode_count);
    block_count = fs_get_used_blocks(shm_addr);
    TEST_ASSERT_EQUAL(1, block_count);
    bit_set = is_bit_set(2, fs_get_inode_bitmap_ptr(shm_addr));
    TEST_ASSERT_EQUAL(0, bit_set);
    bit_set = is_bit_set(1, fs_get_block_bitmap_ptr(shm_addr));
    TEST_ASSERT_EQUAL(0, bit_set);
}

void try_to_remove_removed_dir() {
    int ret_value = simplefs_mkdir("/a");
    TEST_ASSERT_EQUAL(0, ret_value);

    ret_value = simplefs_rmdir("/a");
    TEST_ASSERT_EQUAL(0, ret_value);

    ret_value = simplefs_rmdir("/a");
    TEST_ASSERT_EQUAL(ENOENT, ret_value);

}

void try_to_remove_not_dir() {
    int ret_value = simplefs_creat("/notdir", 0);
    TEST_ASSERT_EQUAL(0, ret_value);
    simplefs_close(ret_value);

    ret_value = simplefs_rmdir("/notdir");
    TEST_ASSERT_EQUAL(ENOTDIR, ret_value);
}

void remove_nested_dirs() {
    simplefs_mkdir("/a");
    simplefs_mkdir("/a/b");
    simplefs_mkdir("/a/b/c");

    uint16_t inode_count = fs_get_used_inodes(shm_addr);
    TEST_ASSERT_EQUAL(5, inode_count);
    uint32_t block_count = fs_get_used_blocks(shm_addr);
    TEST_ASSERT_EQUAL(4, block_count);

    int ret_value = simplefs_rmdir("/a/b/c");
    TEST_ASSERT_EQUAL(0, ret_value);

    ret_value = simplefs_rmdir("/a/b");
    TEST_ASSERT_EQUAL(0, ret_value);

    ret_value = simplefs_rmdir("/a");
    TEST_ASSERT_EQUAL(0, ret_value);

    inode_count = fs_get_used_inodes(shm_addr);
    TEST_ASSERT_EQUAL(2, inode_count);
    block_count = fs_get_used_blocks(shm_addr);
    TEST_ASSERT_EQUAL(1, block_count);
}


int main(void) {
    UNITY_BEGIN();

    RUN_TEST(remove_root_or_1char_dir);
    RUN_TEST(remove_not_root_subdir);
    RUN_TEST(too_long_filename);
    RUN_TEST(remove_non_existent_file);
    RUN_TEST(remove_not_empty_dir);
    RUN_TEST(remove_one_dir);
    RUN_TEST(try_to_remove_removed_dir);
    RUN_TEST(try_to_remove_not_dir);
    RUN_TEST(remove_nested_dirs);

    return UNITY_END();
}
