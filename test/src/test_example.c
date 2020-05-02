// Each test filename should be like 'test_*.c'.
// Otherwise makefile won't be able to find them. 


// include Unity
#include "unity.h"

// include library to test
#include "simplefs_api.h"

// runs before each test
void setUp(void) {
    // set stuff up here
}

// runs after each test
void tearDown(void) {
    // clean stuff up here
}

// tests
void my_test(void)
{
  TEST_ASSERT_EQUAL(0, simplefs_open("somefile", 0));
}

void my_another_test(void)
{
  TEST_ASSERT_EQUAL(0, simplefs_open("another/file", 1));
}

int main(void) {

    // start main with unity_begin
    UNITY_BEGIN();

    // run your tests
    RUN_TEST(my_test);
    RUN_TEST(my_another_test);
    
    // return unity_end
    return UNITY_END();
}
