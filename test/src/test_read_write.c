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

void write_file_not_in_write_mode(){
    char buf[100];
    int fd = simplefs_creat("/read_mode", FS_READ);
    int result = simplefs_write(fd, &buf, sizeof(buf));
    TEST_ASSERT_EQUAL(EBADF, result);
}

void read_file_not_in_read_mode(){
    char buf[100];
    int fd = simplefs_creat("/write_mode", FS_WRITE);
    int result = simplefs_read(fd, &buf, sizeof(buf));
    TEST_ASSERT_EQUAL(EBADF, result);
}

void successfully_write_file_and_return_written_bytes_count(){
    char buf[100] = "123456789";
    int fd = simplefs_creat("/write_mode_count", FS_WRITE);
    int result = simplefs_write(fd, &buf, sizeof(buf));
    TEST_ASSERT_EQUAL(100, result);
}

void successfully_read_file_and_return_read_bytes_count(){
    char buf[100] = "123456789";
    char buf_read[50];
    int fd = simplefs_creat("/read_mode_count", FS_WRITE);
    int result = simplefs_write(fd, &buf, sizeof(buf));
    int fd_open = simplefs_open("/read_mode_count", FS_READ);
    int read_result = simplefs_read(fd_open, &buf_read, sizeof(buf_read));
    TEST_ASSERT(!strcmp(buf_read, "123456789"));
    TEST_ASSERT_EQUAL(50, read_result);
}

void read_file_more_bytes_than_buffer_size(){
    char buf[100] = "123456789";
    char buf_read[50];
    int fd = simplefs_creat("/read_mode_count", FS_WRITE);
    int result = simplefs_write(fd, &buf, sizeof(buf));
    int fd_open = simplefs_open("/read_mode_count", FS_READ);
    int read_result = simplefs_read(fd_open, &buf_read, 100);
    TEST_ASSERT(!strcmp(buf_read, "123456789"));
    TEST_ASSERT_EQUAL(50, read_result);
}



int main(void) {
    UNITY_BEGIN();

    RUN_TEST(read_file_not_in_read_mode);
    RUN_TEST(write_file_not_in_write_mode);
    RUN_TEST(successfully_write_file_and_return_written_bytes_count);
    RUN_TEST(successfully_read_file_and_return_read_bytes_count);
    RUN_TEST(read_file_more_bytes_than_buffer_size);

    return UNITY_END();
}
