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

void test_open_close(void){
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


int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_open_close);
    
    return UNITY_END();
}