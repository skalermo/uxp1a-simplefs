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

const unsigned int max = 32;

void test_creat_close(void){
    int fd[max];

    for(int i = 0; i < max; ++i){
        char file[256];
        snprintf(file, sizeof file, "%s%d%s", "/file", i, ".txt");
        fd[i] = simplefs_creat(file, 0);

        TEST_ASSERT_GREATER_OR_EQUAL(0, fd[i]);
    }

    for(int i = 0; i < max; ++i){
        TEST_ASSERT_GREATER_OR_EQUAL(0, simplefs_close(fd[i]));
    }
}

void test_open(void){
    int fd[max];

    for(int i = 0; i < max; ++i){
        char file[256];
        snprintf(file, sizeof file, "%s%d%s", "/file", i, ".txt");
        fd[i] = simplefs_open(file, 0);

        TEST_ASSERT_GREATER_OR_EQUAL(0, fd[i]);
    }

    for(int i = 0; i < max; ++i){
        TEST_ASSERT_GREATER_OR_EQUAL(0, simplefs_close(fd[i]));
    }
}

void test_unlink(void){
    for(int i = 0; i < max; ++i){
        char file[256];
        snprintf(file, sizeof file, "%s%d%s", "/file", i, ".txt");

        int ret = simplefs_unlink(file);
        if(ret < 0){
            printf("%s\n", file);
        }

        TEST_ASSERT_GREATER_OR_EQUAL(0, ret);
    }
}

void test_mix(void){
    // open not existing
    int fd = simplefs_open("/file.txt", 0); //ENOTDIR
    TEST_ASSERT_EQUAL(ENOTDIR, fd);

    fd = simplefs_creat("/file.txt", 0); // 0
    TEST_ASSERT_EQUAL(0, fd);

    int ret = simplefs_close(fd);   // 0
    TEST_ASSERT_EQUAL(0, ret);

    fd = simplefs_open("/file.txt", 0); // 0
    TEST_ASSERT_EQUAL(0, fd);

    ret = simplefs_unlink("/file.txt"); //EBUSY
    TEST_ASSERT_EQUAL(EBUSY, ret);

    ret = simplefs_close(fd);               // 0
    TEST_ASSERT_EQUAL(0, ret);

    ret = simplefs_unlink("/file.txt"); // 0
    TEST_ASSERT_EQUAL(0, ret);

    fd = simplefs_open("/file.txt", 0); // ENOTDIR
    TEST_ASSERT_EQUAL(ENOTDIR, fd);

    fd = simplefs_creat("/file.txt", 0); //0
    TEST_ASSERT_EQUAL(0, fd);

    ret = simplefs_close(fd);                       // 0
    TEST_ASSERT_EQUAL(0, ret);

    ret = simplefs_unlink("/file.txt"); // 0
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