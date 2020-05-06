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
    int firstBit = __builtin_clz(bitmap[i]) - 23; // 23 = 16 + 8 - 1

    return firstBit < 8 ? 8 * i + firstBit : 0; // 8 = uint8_t
}
