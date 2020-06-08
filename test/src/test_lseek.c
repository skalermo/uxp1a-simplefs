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


void lseek_on_negative_descriptor() {
    int ret_value = simplefs_lseek(-1, SEEK_CUR, 10);
    TEST_ASSERT_EQUAL(EBADF, ret_value);
}

void check_if_lseek_change_openfile() {
    int fd = simplefs_creat("/file", RDWR);
    TEST_ASSERT_EQUAL(0, fd);

    int ret_value = simplefs_lseek(fd, SEEK_SET, 4);
    TEST_ASSERT_EQUAL(4, ret_value);

    uint32_t offset = 0;
    fs_get_data_from_open_file_uint32(fd, 2, &offset, shm_addr);
    TEST_ASSERT_EQUAL(4, offset);

}

void write_to_file_then_set_seek() {
    char *buf = "Hello world!";
    int fd = simplefs_creat("/file", RDWR);
    TEST_ASSERT_EQUAL(0, fd);

    int ret_value = simplefs_write(fd, buf, strlen(buf));
    TEST_ASSERT_EQUAL(strlen(buf), ret_value);

    ret_value = simplefs_lseek(fd, SEEK_SET, 0);
    TEST_ASSERT_EQUAL(0, ret_value);

    char buf_to_read[20];
    ret_value = simplefs_read(fd, buf_to_read, 20);
    buf_to_read[ret_value] = '\0';
    TEST_ASSERT_EQUAL(strlen(buf), ret_value);
    TEST_ASSERT_EQUAL_STRING(buf, buf_to_read);

}

void seek_cur() {
    char *buf = "Hello world!";
    int fd = simplefs_creat("/file", RDWR);

    int ret_value = simplefs_write(fd, buf, strlen(buf));
    TEST_ASSERT_EQUAL(strlen(buf), ret_value);

    ret_value = simplefs_lseek(fd, SEEK_CUR, -1);
    TEST_ASSERT_EQUAL(strlen(buf)-1, ret_value);

    char buf_to_read[20];
    ret_value = simplefs_read(fd, buf_to_read, 20);
    buf_to_read[ret_value] = '\0';
    TEST_ASSERT_EQUAL_STRING("!", buf_to_read);
}

void lseek_leads_to_negative_offset() {
    char *buf = "Hello world!";
    int fd = simplefs_creat("/file", RDWR);

    simplefs_write(fd, buf, strlen(buf));
    int ret_value = simplefs_lseek(fd, SEEK_CUR, -100);
    TEST_ASSERT_EQUAL(EINVAL, ret_value);
}

void set_offset_behind_file_end() {
    char *buf = "Hello world!";
    int fd = simplefs_creat("/file", RDWR);

    simplefs_write(fd, buf, strlen(buf));
    int ret_value = simplefs_lseek(fd, SEEK_CUR, 10);

    TEST_ASSERT_EQUAL(strlen(buf) + 10, ret_value);
    char buf_to_read[20];
    ret_value = simplefs_read(fd, buf_to_read, 20);
    TEST_ASSERT_EQUAL(0, ret_value);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(lseek_on_negative_descriptor);
    RUN_TEST(check_if_lseek_change_openfile);
    RUN_TEST(write_to_file_then_set_seek);
    RUN_TEST(seek_cur);
    RUN_TEST(lseek_leads_to_negative_offset);
    RUN_TEST(set_offset_behind_file_end);

    return UNITY_END();
}
