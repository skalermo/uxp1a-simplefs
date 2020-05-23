#include "unity.h"
#include "simplefs_utils.h"
#include <stdlib.h>

void setUp(void) {

}


void tearDown(void) {

}

void free_string_array(char*** entries, int dirCount)
{
    for(int i = 0; i < dirCount; ++i )
    {
        free((*entries)[i]);
    }
    free(*entries);
    *entries = NULL;
}

void test_dir_count(void)
{
    int dirCount = 0;
    char** entries = NULL;


    dirCount = parse_path("/abc/cd/e", &entries);
    TEST_ASSERT_EQUAL(4, dirCount);
    free_string_array(&entries, dirCount);

    //path given as NULL string
    dirCount = parse_path(NULL, &entries);
    TEST_ASSERT_EQUAL(EINPATH, dirCount);
    free_string_array(&entries, dirCount);

    //path is not absolute
    dirCount = parse_path("fsxc/fds/frwq/fwq", &entries);
    TEST_ASSERT_EQUAL(EINPATH, dirCount);
    free_string_array(&entries, dirCount);

}

void test_parsed_dirs(void)
{
    int dirCount = 0;
    char** entries = NULL;

    dirCount = parse_path("/abc/cd/e", &entries);
    TEST_ASSERT_EQUAL_STRING("/", entries[0]);
    TEST_ASSERT_EQUAL_STRING("abc", entries[1]);
    TEST_ASSERT_EQUAL_STRING("cd", entries[2]);
    TEST_ASSERT_EQUAL_STRING("e", entries[3]);
    free_string_array(&entries, dirCount);

    dirCount = parse_path(NULL, &entries);
    TEST_ASSERT_EQUAL(NULL, entries);
}


int main(void) {


    UNITY_BEGIN();


    RUN_TEST(test_dir_count);
    RUN_TEST(test_parsed_dirs);


    return UNITY_END();
}