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

void successfully_write_file_twice(){
    char buf_1[10] = "123456789";
    char buf_2[4] = "123";
    int fd = simplefs_creat("/write_mode_count_twice", FS_WRITE);
    int result_1 = simplefs_write(fd, &buf_1, sizeof(buf_1));
    int result_2 = simplefs_write(fd, &buf_2, sizeof(buf_2));
    TEST_ASSERT_EQUAL(10, result_1);
    TEST_ASSERT_EQUAL(4, result_2);
}



void successfully_read_file_and_return_read_bytes_count(){
    char buf[100] = "123456789";
    char buf_read[50];
    int fd = simplefs_creat("/read_write_count", FS_WRITE);
    int result = simplefs_write(fd, &buf, sizeof(buf));
    int fd_open = simplefs_open("/read_write_count", FS_READ);
    int read_result = simplefs_read(fd_open, &buf_read, sizeof(buf_read));
    TEST_ASSERT(!strcmp(buf_read, "123456789"));
    TEST_ASSERT_EQUAL(50, read_result);
}

void read_file_twice_gives_correct_result(){
    char buf[100] = "123456789";
    char buf_read_1[100];
    char buf_read_2[100];
    int fd = simplefs_creat("/read_mode_twice", FS_WRITE);
    int result = simplefs_write(fd, &buf, sizeof(buf));
    int fd_open = simplefs_open("/read_mode_twice", FS_READ);
    int read_result_1 = simplefs_read(fd_open, &buf_read_1, 100);
    int read_result_2 = simplefs_read(fd_open, &buf_read_2, 100);
    TEST_ASSERT(!strcmp(buf_read_1, "123456789"));
    TEST_ASSERT(!strcmp(buf_read_2, ""));
    TEST_ASSERT_EQUAL(read_result_1, read_result_2);
}

void read_file_more_bytes_than_buffer_size(){
    char buf[100] = "123456789";
    char buf_read[50];
    int fd = simplefs_creat("/read_mode_buf", FS_WRITE);
    int result = simplefs_write(fd, &buf, sizeof(buf));
    int fd_open = simplefs_open("/read_mode_buf", FS_READ);
    int read_result = simplefs_read(fd_open, &buf_read, 100);
    TEST_ASSERT(!strcmp(buf_read, "123456789"));
    TEST_ASSERT_EQUAL(50, read_result);
}


void successfully_write_file_twice_and_read_different_content(){
    char buf_1[10] = "123456789";
    char buf_2[4] = "123";
    char buf_read_1[100];
    char buf_read_2[100];
    int fd = simplefs_creat("/write_mode_twice", FS_WRITE);
    int fd_open = simplefs_open("/write_mode_twice", FS_READ);
    simplefs_write(fd, &buf_1, sizeof(buf_1));
    simplefs_read(fd_open, &buf_read_1, 10);
    TEST_ASSERT(!strcmp(buf_read_1, "123456789"));
    simplefs_write(fd, &buf_2, sizeof(buf_2));
    simplefs_read(fd_open, &buf_read_2, 4);
    TEST_ASSERT(!strcmp(buf_read_2, "123"));
}

void write_read_file_multiple_blocks(){
    char buf[601] ="0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
                   "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
                   "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
                   "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
                   "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
                   "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"; //601B
    char buf_read[601];
    int fd = simplefs_creat("/multiple_blocks", FS_WRITE);
    int result = simplefs_write(fd, &buf, 601);
    int fd_open = simplefs_open("/multiple_blocks", FS_READ);
    int read_result = simplefs_read(fd_open, &buf_read, 601);
    TEST_ASSERT(!strcmp(buf_read, buf));
}



int main(void) {
    UNITY_BEGIN();

    RUN_TEST(read_file_not_in_read_mode);
    RUN_TEST(write_file_not_in_write_mode);
    RUN_TEST(successfully_write_file_and_return_written_bytes_count);
    RUN_TEST(successfully_read_file_and_return_read_bytes_count);
    RUN_TEST(read_file_more_bytes_than_buffer_size);
    RUN_TEST(read_file_twice_gives_correct_result);
    RUN_TEST(successfully_write_file_twice_and_read_different_content);
    RUN_TEST(successfully_write_file_twice);
    RUN_TEST(write_read_file_multiple_blocks);

    return UNITY_END();
}
