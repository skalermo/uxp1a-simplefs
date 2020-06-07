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

void write_to_file_then_set_seek() {
    char *buf = "Hello world!";
    int fd = simplefs_creat("/file", RDWR);
    int ret_value = simplefs_write(fd, buf, strlen(buf));
    TEST_ASSERT_EQUAL(strlen(buf), ret_value);

    ret_value = simplefs_lseek(fd, SEEK_SET, 0);
    TEST_ASSERT_EQUAL(0, ret_value);

    char buf_to_read[20];
    ret_value = simplefs_write(fd, buf_to_read, strlen(buf_to_read));
    TEST_ASSERT_EQUAL(strlen(buf), ret_value);
    TEST_ASSERT_EQUAL_STRING(buf, buf_to_read);


}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(lseek_on_negative_descriptor);
    RUN_TEST(write_to_file_then_set_seek);


    return UNITY_END();
}
