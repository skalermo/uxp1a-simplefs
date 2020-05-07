#include "simplefs_utils.h"

/**
 * @brief Find free bit
 * @param bitmap
 * @param size
 * @return index or -1 if not found
 */
int find_free_bit(uint8_t *bitmap, int size){
    // skip all zeros
    for(int i = 0; i < size; ++i){
        if(bitmap[i] != 0){
            // count leading zeros + 1
            int first_bit = __builtin_clz(bitmap[i]) - 23; // 23 = 16 + 8 - 1

            return 8 * i + first_bit - 1; // 8 = uint8_t
        }
    }
    return -1;
}

/**
 * @brief Set bit in bitmap
 * @param bitmap
 * @param index
 */
void set_bit(uint8_t *bitmap, int index){
    int bitmap_idx = index / 8; // 8 = uint8_t
    int idx = index % 8;

    uint8_t one_hot = 0b10000000;
    for(int i = 0; i < idx; ++i){ one_hot >>=1;} // One Hot encode idx

    bitmap[bitmap_idx] |= one_hot;
}

/**
 * @brief Unset bit in bitmap
 * @param bitmap
 * @param index
 */
void unset_bit(uint8_t *bitmap, int index){
    int bitmap_idx = index / 8; // 8 = uint8_t
    int idx = index % 8;

    uint8_t one_hot = 0b10000000;
    for(int i = 0; i < idx; ++i){ one_hot >>=1;} // One Hot encode idx

    bitmap[bitmap_idx] &= ~one_hot;
}
