#include "simplefs_utils.h"

/**
 * @brief Find free bit
 * @param bitmap
 * @param size
 * @return index or 0 if not found
 */
int find_free_bit(uint8_t *bitmap, int size){
    // skip all zeros
    int i = 0;
    for(; i < size && bitmap[i] == 0x0; ++i);

    // count leading zeros + 1
    int first_bit = __builtin_clz(bitmap[i]) - 23; // 23 = 16 + 8 - 1

    return first_bit < 8 ? 8 * i + first_bit : 0; // 8 = uint8_t
}

void set_bit(uint8_t *bitmap, int index){
    int bitmap_idx = index / 8; // 8 = uint8_t
    int idx = index % 8;

    uint8_t one_hot = 0b10000000;
    for(int i = 0; i < idx; ++i){ one_hot >>=1;} // One Hot encode idx

    bitmap[bitmap_idx] |= one_hot;
}

void unset_bit(uint8_t *bitmap, int index){
    int bitmap_idx = index / 8; // 8 = uint8_t
    int idx = index % 8;

    uint8_t one_hot = 0b10000000;
    for(int i = 0; i < idx; ++i){ one_hot >>=1;} // One Hot encode idx

    bitmap[bitmap_idx] &= ~one_hot;
}
