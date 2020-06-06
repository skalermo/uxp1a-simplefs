/*
 * test_open_close.c
 *
 *      Author: Kordowski Mateusz
 */

#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/stat.h> 
#include <fcntl.h> 


#include "unity.h"


#include "simplefs_api.h"

void setUp(void){

}

void tearDown(void){

}

void test_creat_close(void){
    int fd[6];

    TEST_ASSERT_GREATER_OR_EQUAL(0, fd[0] = simplefs_creat("/test_open_close1", 0));
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd[1] = simplefs_creat("/test_open_close2", 0));
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd[2] = simplefs_creat("/test_open_close3", 0));
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd[3] = simplefs_creat("/test_open_close4", 0));
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd[4] = simplefs_creat("/test_open_close5", 0));
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd[5] = simplefs_creat("/test_open_close6", 0));

    TEST_ASSERT_GREATER_OR_EQUAL(0, simplefs_close(fd[0]));
    TEST_ASSERT_GREATER_OR_EQUAL(0, simplefs_close(fd[1]));
    TEST_ASSERT_GREATER_OR_EQUAL(0, simplefs_close(fd[2]));
    TEST_ASSERT_GREATER_OR_EQUAL(0, simplefs_close(fd[3]));
    TEST_ASSERT_GREATER_OR_EQUAL(0, simplefs_close(fd[4]));
    TEST_ASSERT_GREATER_OR_EQUAL(0, simplefs_close(fd[5]));

}

void test_open(void){
    int fd[6];

    TEST_ASSERT_GREATER_OR_EQUAL(0, fd[0] = simplefs_open("/test_open_close1", 0));
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd[1] = simplefs_open("/test_open_close2", 0));
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd[2] = simplefs_open("/test_open_close3", 0));
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd[3] = simplefs_open("/test_open_close4", 0));
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd[4] = simplefs_open("/test_open_close5", 0));
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd[5] = simplefs_open("/test_open_close6", 0));

    TEST_ASSERT_GREATER_OR_EQUAL(0, simplefs_close(fd[0]));
    TEST_ASSERT_GREATER_OR_EQUAL(0, simplefs_close(fd[1]));
    TEST_ASSERT_GREATER_OR_EQUAL(0, simplefs_close(fd[2]));
    TEST_ASSERT_GREATER_OR_EQUAL(0, simplefs_close(fd[3]));
    TEST_ASSERT_GREATER_OR_EQUAL(0, simplefs_close(fd[4]));
    TEST_ASSERT_GREATER_OR_EQUAL(0, simplefs_close(fd[5]));
}

void test_unlink(void){
    TEST_ASSERT_GREATER_OR_EQUAL(0, simplefs_unlink("/test_open_close1"));
    TEST_ASSERT_GREATER_OR_EQUAL(0, simplefs_unlink("/test_open_close2"));
    TEST_ASSERT_GREATER_OR_EQUAL(0, simplefs_unlink("/test_open_close3"));
    TEST_ASSERT_GREATER_OR_EQUAL(0, simplefs_unlink("/test_open_close4"));
    TEST_ASSERT_GREATER_OR_EQUAL(0, simplefs_unlink("/test_open_close5"));
    TEST_ASSERT_GREATER_OR_EQUAL(0, simplefs_unlink("/test_open_close6"));
}

void test_mix(void){
    // open not existing
    int fd = simplefs_open("/file.txt", FS_READ); //ENOTDIR
    TEST_ASSERT_EQUAL(ENOTDIR, fd);

    fd = simplefs_creat("/file.txt", FS_READ); // 0
    TEST_ASSERT_EQUAL(0, fd);

    int ret = simplefs_close(fd);   // 0
    TEST_ASSERT_EQUAL(0, ret);

    fd = simplefs_open("/file.txt", FS_READ); // 0
    TEST_ASSERT_EQUAL(0, fd);

    ret = simplefs_unlink("/file.txt"); //EBUSY
    TEST_ASSERT_EQUAL(EBUSY, ret);

    ret = simplefs_close(fd);               // 0
    TEST_ASSERT_EQUAL(0, ret);

    ret = simplefs_unlink("/file.txt"); // 0
    TEST_ASSERT_EQUAL(0, ret);

    fd = simplefs_open("/file.txt", FS_READ); // ENOTDIR
    TEST_ASSERT_EQUAL(ENOTDIR, fd);

    fd = simplefs_creat("/file.txt", FS_READ); //0
    TEST_ASSERT_EQUAL(0, fd);

    ret = simplefs_close(fd);                       // 0
    TEST_ASSERT_EQUAL(0, ret);
}


int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_creat_close);
    RUN_TEST(test_open);
    RUN_TEST(test_unlink);
    RUN_TEST(test_mix);

    return UNITY_END();
}