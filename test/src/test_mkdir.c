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
    printf("%p\n", ptr);
    TEST_ASSERT_EQUAL(3, inode_count);
    TEST_ASSERT_EQUAL(0, ret_value);

    unlink_fs();
}

void make_three_dirs() {
    // get_ptr_to_fs creates fs
    void *ptr = get_ptr_to_fs();
    uint16_t inode_count = fs_get_used_inodes(ptr);
    printf("%p\n", ptr);
    TEST_ASSERT_EQUAL(2, inode_count);

    int ret_value = simplefs_mkdir("/a");
    inode_count = fs_get_used_inodes(ptr);
    TEST_ASSERT_EQUAL(0, ret_value); // <- ret_value == -13
    TEST_ASSERT_EQUAL(3, inode_count);

//    ret_value = simplefs_mkdir("/b");
//    inode_count = fs_get_used_inodes(ptr);
//    TEST_ASSERT_EQUAL(4, inode_count);
//    TEST_ASSERT_EQUAL(0, ret_value);
//
//    ret_value = simplefs_mkdir("/c");
//    inode_count = fs_get_used_inodes(ptr);
//    TEST_ASSERT_EQUAL(5, inode_count);
//    TEST_ASSERT_EQUAL(0, ret_value);

    unlink_fs();
}

void make_dir() {
    // get_ptr_to_fs creates fs
    void *ptr = get_ptr_to_fs();
    uint16_t inode_count = fs_get_used_inodes(ptr);
    printf("%u\n", inode_count);

    int ret_val = simplefs_mkdir("/a");
    printf("%d\n", ret_val);
    ret_val = simplefs_mkdir("/b");
    printf("%d\n", ret_val);
    inode_count = fs_get_used_inodes(ptr);
    printf("%u\n", inode_count);

    unlink_fs();
}


int main(void) {


    UNITY_BEGIN();

    RUN_TEST(make_one_dir);
    RUN_TEST(make_three_dirs);


    return UNITY_END();
}