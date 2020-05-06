#ifndef SIMPLEFS_UTILS_C
#define SIMPLEFS_UTILS_C

#include <stdint.h>

#define member_size(type, member) sizeof(((type *)0)->member)

uint32_t inner_fs_get_position_in_8bit(uint8_t bits){
    uint8_t shift = 0x1;
    uint32_t ret = 0;
    for(; shift != 0; shift = 1 << shift, ++ret){
        if(bits & shift != 0) return ret;
    }
    return UINT32_MAX;
}

uint32_t inner_fs_get_position_in_16bit(uint16_t bits){
    uint32_t ret = 0;
    uint8_t fun;

    fun = 8 >> (bits & 0xFF00);
    if(fun != 0) {
        ret = inner_fs_get_position_in_8bit(fun);
        if(ret == UINT32_MAX) return UINT32_MAX;
        return ret + 8;
    }

    fun = bits & 0x00FF;
    if(fun != 0) return inner_fs_get_position_in_8bit(fun);

    return UINT32_MAX;
}

uint32_t inner_fs_get_position_in_32bit(uint32_t bits){
    uint32_t ret = 0;
    uint16_t fun;

    fun = 16 >> (bits & 0xFFFF0000);
    if(fun != 0) {
        ret =  inner_fs_get_position_in_16bit(fun);
        if(ret == UINT32_MAX) return UINT32_MAX;
        return ret + 16;
    }

    fun = bits & 0x0000FFFF;
    if(fun != 0) return inner_fs_get_position_in_16bit(fun);

    return UINT32_MAX;
}

/**
 * @brief Find index of some free inode.
 * 
 * @param addr - address of the mapped shared memory.
 * @return uint32_t - UINT32_MAX if some error happend or
 * 0 if there is no more free inodes.
 * Otherwise index of an free inode.
 */



/**
 * @brief Find index of some free struct.
 * 
 * @param block_ptr - pointer to the bitmap block.
 * @param maxOffset - maximum number of some structures in block. All of those indexed structures must be usable.
 * @return uint32_t - UINT32_MAX if some error happend or
 * 0 if there is no more free inodes.
 * Otherwise index of an free inode.
 */
uint32_t inner_fs_find_free_index(void* bitmap_ptr, uint32_t maxOffset){
    uint32_t offset = 0;

    uint32_t test32;
    uint16_t test16;
    uint8_t test8;

    char found = 0;

    uint32_t ret = 0;

    for(; offset <= maxOffset - 32; offset += 32){
        memcpy(&test32, bitmap_ptr + offset, sizeof(uint32_t));
        if(test32 != 0) {
            found = 1;
            break;
        }
    }

    if(!found) {    // there still might be free inode somewhere at the end
        offset -= 32;
        for(; offset <= maxOffset - 8; offset += 8){
            memcpy(&test8, bitmap_ptr + offset, sizeof(uint8_t));
            if(test8 != 0) {
                ret = inner_fs_get_position_in_8bit(test8);
                if(ret == UINT32_MAX) return UINT32_MAX; // error
                return offset + ret;
            }
        }

        // not found

        return 0;
    }
    else{
        ret = inner_fs_get_position_in_32bit(test32);
        if(ret == UINT32_MAX) return UINT32_MAX; // error

        return offset + ret;
    }

}

int8_t inner_fs_mark_bitmap_bit(void* bitmap_ptr, uint32_t bitmapIndex){
    bitmap_ptr += bitmapIndex / 8;
    uint32_t smallBitmapOffset = bitmapIndex % 8;

    uint8_t bitmapCopy;
    memcpy(&bitmapCopy, bitmap_ptr, sizeof(uint8_t));

    uint8_t setBit = 1;

    setBit = smallBitmapOffset << setBit;
    bitmapCopy = bitmapCopy | setBit;

    memcpy(bitmap_ptr, &bitmapCopy, sizeof(uint8_t));

    return 0;
}

int8_t inner_fs_free_bitmap_bit(void* bitmap_ptr, uint32_t bitmapIndex){
    bitmap_ptr += bitmapIndex / 8;
    uint32_t smallBitmapOffset = bitmapIndex % 8;

    uint8_t bitmapCopy;
    memcpy(&bitmapCopy, bitmap_ptr, sizeof(uint8_t));

    uint8_t setBit = 1;

    setBit = smallBitmapOffset << setBit;
    bitmapCopy = bitmapCopy & setBit;

    memcpy(bitmap_ptr, &bitmapCopy, sizeof(uint8_t));

    return 0;
}

#endif