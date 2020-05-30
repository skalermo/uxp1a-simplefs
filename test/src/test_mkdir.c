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


void make_one_dir() {

    uint16_t inode_count = fs_get_used_inodes(shm_addr);
    TEST_ASSERT_EQUAL(2, inode_count);

    int ret_value = simplefs_mkdir("/a");
    inode_count = fs_get_used_inodes(shm_addr);
    TEST_ASSERT_EQUAL(3, inode_count);
    TEST_ASSERT_EQUAL(0, ret_value);

}


void make_three_dirs() {

    uint16_t inode_count = fs_get_used_inodes(shm_addr);
    TEST_ASSERT_EQUAL(2, inode_count);

    int ret_value = simplefs_mkdir("/a");
    inode_count = fs_get_used_inodes(shm_addr);
    TEST_ASSERT_EQUAL(0, ret_value);
    TEST_ASSERT_EQUAL(3, inode_count);

    ret_value = simplefs_mkdir("/b");
    inode_count = fs_get_used_inodes(shm_addr);
    TEST_ASSERT_EQUAL(4, inode_count);
    TEST_ASSERT_EQUAL(0, ret_value);

    ret_value = simplefs_mkdir("/c");
    inode_count = fs_get_used_inodes(shm_addr);
    TEST_ASSERT_EQUAL(5, inode_count);
    TEST_ASSERT_EQUAL(0, ret_value);

}

void make_duplicate_dir() {

    // create new dir "a"
    int ret_value = simplefs_mkdir("/a");
    uint16_t inode_count = fs_get_used_inodes(shm_addr);
    TEST_ASSERT_EQUAL(0, ret_value);
    TEST_ASSERT_EQUAL(3, inode_count);

    // try to create dir "a" once again
    ret_value = simplefs_mkdir("/a");
    inode_count = fs_get_used_inodes(shm_addr);
    TEST_ASSERT_EQUAL(EEXIST, ret_value);
    TEST_ASSERT_EQUAL(3, inode_count);

    // create new dir "b"
    ret_value = simplefs_mkdir("/b");
    inode_count = fs_get_used_inodes(shm_addr);
    TEST_ASSERT_EQUAL(0, ret_value);
    TEST_ASSERT_EQUAL(4, inode_count);

    // try to create dir "b" once again
    ret_value = simplefs_mkdir("/b");
    inode_count = fs_get_used_inodes(shm_addr);
    TEST_ASSERT_EQUAL(EEXIST, ret_value);
    TEST_ASSERT_EQUAL(4, inode_count);

}

void nested_dirs() {

    int ret_value = simplefs_mkdir("/a");
    uint16_t inode_count = fs_get_used_inodes(shm_addr);
    TEST_ASSERT_EQUAL(0, ret_value);
    TEST_ASSERT_EQUAL(3, inode_count);

    // create dir "b" inside dir "a"
    ret_value = simplefs_mkdir("/a/b");
    inode_count = fs_get_used_inodes(shm_addr);
    TEST_ASSERT_EQUAL(0, ret_value);
    TEST_ASSERT_EQUAL(4, inode_count);

    // create dir "c" inside dir "a"
    ret_value = simplefs_mkdir("/a/c");
    inode_count = fs_get_used_inodes(shm_addr);
    TEST_ASSERT_EQUAL(0, ret_value);
    TEST_ASSERT_EQUAL(5, inode_count);

    // create dir "d" inside dir "c"
    ret_value = simplefs_mkdir("/a/c/d");
    inode_count = fs_get_used_inodes(shm_addr);
    TEST_ASSERT_EQUAL(0, ret_value);
    TEST_ASSERT_EQUAL(6, inode_count);

}

void make_duplicate_in_nested_dir() {

    int ret_value = simplefs_mkdir("/a");
    uint16_t inode_count = fs_get_used_inodes(shm_addr);
    TEST_ASSERT_EQUAL(0, ret_value);
    TEST_ASSERT_EQUAL(3, inode_count);

    // create dir "b" inside dir "a"
    ret_value = simplefs_mkdir("/a/b");
    inode_count = fs_get_used_inodes(shm_addr);
    TEST_ASSERT_EQUAL(0, ret_value);
    TEST_ASSERT_EQUAL(4, inode_count);

    // try to create dup dir "b" inside dir "a"
    ret_value = simplefs_mkdir("/a/b");
    inode_count = fs_get_used_inodes(shm_addr);
    TEST_ASSERT_EQUAL(EEXIST, ret_value);
    TEST_ASSERT_EQUAL(4, inode_count);

    // create dir "c" inside dir "b"
    ret_value = simplefs_mkdir("/a/b/c");
    inode_count = fs_get_used_inodes(shm_addr);
    TEST_ASSERT_EQUAL(0, ret_value);
    TEST_ASSERT_EQUAL(5, inode_count);

    // try to create dup dir "c" inside dir "b"
    ret_value = simplefs_mkdir("/a/b/c");
    inode_count = fs_get_used_inodes(shm_addr);
    TEST_ASSERT_EQUAL(EEXIST, ret_value);
    TEST_ASSERT_EQUAL(5, inode_count);

}


void try_to_create_dir_not_in_root_subdir() {

    int ret_value = simplefs_mkdir("some_dirname");
    uint16_t inode_count = fs_get_used_inodes(shm_addr);

    TEST_ASSERT_EQUAL(ENOENT, ret_value);
    TEST_ASSERT_EQUAL(2, inode_count);

}

void try_to_create_dir_with_too_long_name() {

    // ok_name == '/' + 'a' * 61
    // len(ok_name) - len('/') == 61 == FS_NAME_SIZE
    char *ok_name = "/"
                 "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                 "aaaaaaaaaaa";

    int ret_value = simplefs_mkdir(ok_name);
    uint16_t inode_count = fs_get_used_inodes(shm_addr);

    TEST_ASSERT_EQUAL(0, ret_value);
    TEST_ASSERT_EQUAL(3, inode_count);

    // too_long_name == '/' + 'a' * 61 + 'b'
    // len(ok_name) - len('/') == 62 > FS_NAME_SIZE
    char *too_long_name = "/"
                          "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                          "aaaaaaaaaaab";
    ret_value = simplefs_mkdir(too_long_name);
    inode_count = fs_get_used_inodes(shm_addr);

    TEST_ASSERT_EQUAL(ENAMETOOLONG, ret_value);
    TEST_ASSERT_EQUAL(3, inode_count);
}


void try_to_create_dir_with_too_long_path() {
    // dir_a == '/' + 'a' * 50
    char *dir_a = "/aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    int ret_value = simplefs_mkdir(dir_a);
    uint16_t inode_count = fs_get_used_inodes(shm_addr);
    TEST_ASSERT_EQUAL(0, ret_value);
    TEST_ASSERT_EQUAL(3, inode_count);

    // dir_b == dir_a + '/' + 'b' * 50
    char *dir_b = "/aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                  "/bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";
    ret_value = simplefs_mkdir(dir_b);
    inode_count = fs_get_used_inodes(shm_addr);
    TEST_ASSERT_EQUAL(0, ret_value);
    TEST_ASSERT_EQUAL(4, inode_count);

    // dir_c == dir_b + '/' + 'c' * 50
    char *dir_c = "/aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                  "/bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"
                  "/cccccccccccccccccccccccccccccccccccccccccccccccccc";
    ret_value = simplefs_mkdir(dir_c);
    inode_count = fs_get_used_inodes(shm_addr);
    TEST_ASSERT_EQUAL(0, ret_value);
    TEST_ASSERT_EQUAL(5, inode_count);

    // dir_d == dir_c + '/' + 'd' * 50
    char *dir_d = "/aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                  "/bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"
                  "/cccccccccccccccccccccccccccccccccccccccccccccccccc"
                  "/dddddddddddddddddddddddddddddddddddddddddddddddddd";
    ret_value = simplefs_mkdir(dir_d);
    inode_count = fs_get_used_inodes(shm_addr);
    TEST_ASSERT_EQUAL(0, ret_value);
    TEST_ASSERT_EQUAL(6, inode_count);

    // dir_e == dir_d + '/' + 'e' * 50
    // len(dir_e) == 255
    char *dir_e = "/aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                  "/bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"
                  "/cccccccccccccccccccccccccccccccccccccccccccccccccc"
                  "/dddddddddddddddddddddddddddddddddddddddddddddddddd"
                  "/eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee";
    ret_value = simplefs_mkdir(dir_e);
    inode_count = fs_get_used_inodes(shm_addr);
    TEST_ASSERT_EQUAL(0, ret_value);
    TEST_ASSERT_EQUAL(7, inode_count);

    // dir_f == dir_e + 'f'
    // len(dir_f) == 256 > FS_PATH_MAX
    char *dir_f = "/aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                  "/bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"
                  "/cccccccccccccccccccccccccccccccccccccccccccccccccc"
                  "/dddddddddddddddddddddddddddddddddddddddddddddddddd"
                  "/eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeef";
    ret_value = simplefs_mkdir(dir_f);
    inode_count = fs_get_used_inodes(shm_addr);
    TEST_ASSERT_EQUAL(ENAMETOOLONG, ret_value);
    TEST_ASSERT_EQUAL(7, inode_count);

}


int main(void) {
    UNITY_BEGIN();

    RUN_TEST(make_one_dir);
    RUN_TEST(make_three_dirs);
    RUN_TEST(make_duplicate_dir);
    RUN_TEST(nested_dirs);
    RUN_TEST(make_duplicate_in_nested_dir);
    RUN_TEST(try_to_create_dir_not_in_root_subdir);
    RUN_TEST(try_to_create_dir_with_too_long_name);
    RUN_TEST(try_to_create_dir_with_too_long_path);

    return UNITY_END();
}
