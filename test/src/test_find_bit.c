#include "unity.h"
#include "simplefs_utils.h"
#include <stdint.h>

void setUp(void) {
}

void tearDown(void) {
}

void test_find_generic(void)
{
    // full bitmap
    uint8_t bitmap[2];

    bitmap[0] = 0b00000000;
    bitmap[1] = 0b01110110;


    int idx = find_free_bit(bitmap, 2);

    TEST_ASSERT_EQUAL(10, idx);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_find_generic);

    return UNITY_END();
}





