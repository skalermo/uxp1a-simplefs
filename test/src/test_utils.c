/*
 * test_utils.c
 *
 *      Author: Kordowski Mateusz
 */

#ifndef SIMPLEFS_TEST_UTILS_C
#define SIMPLEFS_INODE_C

#include <string.h>
#include <stdlib.h>

#include "unity.h"
#include "utils.h"

void setUp(void){

}

void tearDown(void){

}

void find_free_bit_test(void){
    uint32_t sizeofBitmap = 48;
    void* bitmap = malloc(sizeofBitmap);
    uint8_t toSave = 0x1B;

    for(uint32_t i = 0; i < sizeofBitmap; ++i){
        memcpy(bitmap + i, &toSave, sizeof(uint8_t));
        ++toSave;
    }
    uint32_t ret = inner_fs_find_free_index(bitmap, sizeofBitmap);

    TEST_ASSERT_TRUE(ret != 0 && ret != UINT32_MAX);
}


int main(void){
    UNITY_BEGIN();

    RUN_TEST(find_free_bit_test);
    
    return UNITY_END();
}



#endif