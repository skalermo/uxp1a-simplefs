#include "unity.h"
#include "simplefs_utils.h"
#include <stdint.h>

void setUp(void) {
}

void tearDown(void) {
}

void test_find_generic(void) {
    uint8_t bitmap[2];

    // full bitmap
    bitmap[0] = 0b00000000;
    bitmap[1] = 0b00000000;

    // test full bitmap
    TEST_ASSERT_EQUAL(-1, find_free_bit(bitmap, 2));

    // set empty spaces as 1
    bitmap[1] = 0b01110110;

    // test with empty spaces
    TEST_ASSERT_EQUAL(9, find_free_bit(bitmap, 2));

    bitmap[0] = 0b01110110;
    TEST_ASSERT_EQUAL(1, find_free_bit(bitmap, 2));
}

void test_bit_flip(void) {
    uint8_t bitmap[2];

    // full bitmap
    bitmap[0] = 0b00000000;
    bitmap[1] = 0b00000000;

    // Change 0 bit to 1
    set_bit(bitmap, 0);
    set_bit(bitmap, 7);
    set_bit(bitmap, 8);
    TEST_ASSERT_EQUAL(129, bitmap[0]);
    TEST_ASSERT_EQUAL(128, bitmap[1]);

    // Change 1 bit to 0
    unset_bit(bitmap, 0);
    unset_bit(bitmap, 8);
    TEST_ASSERT_EQUAL(1, bitmap[0]);
    TEST_ASSERT_EQUAL(0, bitmap[1]);

    // Try to change 1 bit to 1
    set_bit(bitmap, 7);
    TEST_ASSERT_EQUAL(1, bitmap[0]);

    // Try to change 0 bit to 0
    unset_bit(bitmap, 0);
    TEST_ASSERT_EQUAL(1, bitmap[0]);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_find_generic);
    RUN_TEST(test_bit_flip);

    return UNITY_END();
}





