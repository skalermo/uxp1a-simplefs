#include "unity.h"
#include "simplefs_utils.h"
#include <stdint.h>

void setUp(void) {
}

void tearDown(void) {
}

void test_find_generic(void)
{
    uint8_t bitmap[2];

    // full bitmap
    bitmap[0] = 0b00000000;
    bitmap[1] = 0b00000000;

    // test full bitmap
    TEST_ASSERT_EQUAL(0, find_free_bit(bitmap, 2));

    // set empty spaces as 1
    bitmap[1] = 0b01110110;

    // test with empty spaces
    TEST_ASSERT_EQUAL(10, find_free_bit(bitmap, 2));

    bitmap[0] = 0b01110110;
    TEST_ASSERT_EQUAL(2, find_free_bit(bitmap, 2));
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_find_generic);

    return UNITY_END();
}





