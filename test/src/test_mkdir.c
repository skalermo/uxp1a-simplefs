#include "unity.h"
#include "simplefs_utils.h"
#include "init.h"
#include <stdlib.h>
#include <unistd.h>

void setUp(void) {
}


void tearDown(void) {
}


void make_one_dir() {
    // get_ptr_to_fs creates fs
    void *ptr = get_ptr_to_fs();
    uint16_t inode_count = fs_get_used_inodes(ptr);
    TEST_ASSERT_EQUAL(2, inode_count);

    int ret_value = simplefs_mkdir("/a");
    inode_count = fs_get_used_inodes(ptr);
    TEST_ASSERT_EQUAL(3, inode_count);
    TEST_ASSERT_EQUAL(0, ret_value);

    unlink_fs();
}


void make_three_dirs() {
    // get_ptr_to_fs creates fs
    void *ptr = get_ptr_to_fs();

    uint16_t inode_count = fs_get_used_inodes(ptr);
    TEST_ASSERT_EQUAL(2, inode_count);

    int ret_value = simplefs_mkdir("/a");
    inode_count = fs_get_used_inodes(ptr);
    TEST_ASSERT_EQUAL(0, ret_value);
    TEST_ASSERT_EQUAL(3, inode_count);

    ret_value = simplefs_mkdir("/b");
    inode_count = fs_get_used_inodes(ptr);
    TEST_ASSERT_EQUAL(4, inode_count);
    TEST_ASSERT_EQUAL(0, ret_value);

    ret_value = simplefs_mkdir("/c");
    inode_count = fs_get_used_inodes(ptr);
    TEST_ASSERT_EQUAL(5, inode_count);
    TEST_ASSERT_EQUAL(0, ret_value);

    unlink_fs();
}

void make_duplicate_dir() {
    void *ptr = get_ptr_to_fs();

    // create new dir "a"
    int ret_value = simplefs_mkdir("/a");
    uint16_t inode_count = fs_get_used_inodes(ptr);
    TEST_ASSERT_EQUAL(0, ret_value);
    TEST_ASSERT_EQUAL(3, inode_count);

    // try to create dir "a" once again
    ret_value = simplefs_mkdir("/a");
    inode_count = fs_get_used_inodes(ptr);
    TEST_ASSERT_EQUAL(EEXIST, ret_value);
    TEST_ASSERT_EQUAL(3, inode_count);

    // create new dir "b"
    ret_value = simplefs_mkdir("/b");
    inode_count = fs_get_used_inodes(ptr);
    TEST_ASSERT_EQUAL(0, ret_value);
    TEST_ASSERT_EQUAL(4, inode_count);

    // try to create dir "b" once again
    ret_value = simplefs_mkdir("/b");
    inode_count = fs_get_used_inodes(ptr);
    TEST_ASSERT_EQUAL(EEXIST, ret_value);
    TEST_ASSERT_EQUAL(4, inode_count);

    unlink_fs();
}
//
//void nested_dirs() {
//    void *ptr = get_ptr_to_fs();
//
//    int ret_value = simplefs_mkdir("/a");
//    uint16_t inode_count = fs_get_used_inodes(ptr);
//    TEST_ASSERT_EQUAL(0, ret_value);
//    TEST_ASSERT_EQUAL(3, inode_count);
//
//    // create dir "b" inside dir "a"
//    ret_value = simplefs_mkdir("/a/b");
//    inode_count = fs_get_used_inodes(ptr);
//    TEST_ASSERT_EQUAL(0, ret_value);
//    TEST_ASSERT_EQUAL(4, inode_count);
//}


int main(void) {
    UNITY_BEGIN();

    RUN_TEST(make_one_dir);
    RUN_TEST(make_three_dirs);
    RUN_TEST(make_duplicate_dir);
//    RUN_TEST(nested_dirs);


    return UNITY_END();
}
