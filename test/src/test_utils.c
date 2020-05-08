/*
 * test_utils.c
 *
 *      Author: Kordowski Mateusz
 */


#include <string.h>
#include <stdlib.h>

#include "unity.h"
#include "utils.h"

void setUp(void){

}

void tearDown(void){

}

void find_free_bit_test(void){
    uint32_t sizeofBitmap = 32;
    void* bitmap = malloc(sizeofBitmap);
    uint32_t toSave32 = 0x0008000;

    memcpy(bitmap, &toSave32, sizeof(uint32_t));
    uint32_t ret = inner_fs_find_free_index(bitmap, sizeofBitmap);
    TEST_ASSERT_TRUE(ret == 15);

}


int main(void){
    UNITY_BEGIN();

    RUN_TEST(find_free_bit_test);
    
    return UNITY_END();
}

