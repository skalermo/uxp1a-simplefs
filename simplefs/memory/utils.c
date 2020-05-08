
#include "utils.h"

uint32_t inner_fs_get_position_in_8bit(uint8_t bits){
    uint8_t shift = 0x01;
    uint32_t ret = 0;
    for(; ret < 8; shift = shift << 1, ++ret){
        if((bits & shift) != 0) return ret;
    }
    
    return UINT32_MAX;
}

uint32_t inner_fs_get_position_in_16bit(uint16_t bits){
    uint32_t ret = 0;
    uint8_t fun;

    // first half
    fun = bits & 0x00FF;
    if(fun != 0) {
        ret = inner_fs_get_position_in_8bit(fun);
        return ret;
    }

    // second half
    fun = (bits >> 8) & 0x00FF;
    if(fun != 0) {
        ret = inner_fs_get_position_in_8bit(fun);
        if(ret == UINT32_MAX) return UINT32_MAX;
        return ret + 8;
    }

    return UINT32_MAX;
}

uint32_t inner_fs_get_position_in_32bit(uint32_t bits){
    uint32_t ret = 0;
    uint16_t fun;

    // first half
    fun = bits & 0x0000FFFF;
    if(fun != 0) {
        ret =  inner_fs_get_position_in_16bit(fun);
        return ret;
    }

    // second half
    fun = (bits >> 16) & 0x0000FFFF;
    if(fun != 0) { 
        ret = inner_fs_get_position_in_16bit(fun);
        if(ret == UINT32_MAX) return UINT32_MAX;
        return ret + 16;
    }

    return UINT32_MAX;
}

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

        return UINT32_MAX;
    }
    else{
        ret = inner_fs_get_position_in_32bit(test32);
        if(ret == UINT32_MAX) return UINT32_MAX; // error

        return offset + ret;
    }

}

int8_t inner_fs_mark_bitmap_bit(void* bitmap_ptr, uint32_t bitmapIndex){
    bitmap_ptr += bitmapIndex / 8;
    uint8_t smallBitmapOffset = bitmapIndex % 8;

    uint8_t bitmapCopy;
    memcpy(&bitmapCopy, bitmap_ptr, sizeof(uint8_t));
    

    uint8_t setBit = 0x01;
    setBit = setBit <<  smallBitmapOffset;
    bitmapCopy = bitmapCopy | setBit; // set bit
    bitmapCopy = bitmapCopy & (~setBit); // unset bit (0 means it is used)

    memcpy(bitmap_ptr, &bitmapCopy, sizeof(uint8_t));

    return 0;
}

int8_t inner_fs_free_bitmap_bit(void* bitmap_ptr, uint32_t bitmapIndex){
    bitmap_ptr += bitmapIndex / 8;
    uint32_t smallBitmapOffset = bitmapIndex % 8;

    uint8_t bitmapCopy;
    memcpy(&bitmapCopy, bitmap_ptr, sizeof(uint8_t));

    uint8_t setBit = 0x01;

    setBit = setBit << smallBitmapOffset;
    bitmapCopy = bitmapCopy | setBit; // 1 means it is unused

    memcpy(bitmap_ptr, &bitmapCopy, sizeof(uint8_t));

    return 0;
}
