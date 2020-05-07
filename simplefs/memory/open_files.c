/*
 * open_files.c
 *
 *      Author: Kordowski Mateusz
 */

#ifndef SIMPLEFS_OPEN_FILES_C
#define SIMPLEFS_OPEN_FILES_C

#include "open_files.h"

///////////////////////////////////
//  Hidden functions
//////////////////////////////////

uint8_t inner_fs_get_open_file_offsetof(uint8_t index){
    switch(index){
        case 0:
            return offsetof(struct OpenFile, mode);
        case 1:
            return offsetof(struct OpenFile, inode_num);
        case 2:
            return offsetof(struct OpenFile, offset);
        default:
            return UINT8_MAX;
    }
}

uint8_t inner_fs_get_open_file_variable_size(uint8_t index){
    switch(index){
        case 0:
            return member_size(struct OpenFile, mode);
        case 1:
            return member_size(struct OpenFile, inode_num);
        case 2:
            return member_size(struct OpenFile, offset);
        default:
            return UINT8_MAX;
    }
}


///////////////////////////////////
//  Struct functions
//////////////////////////////////


int8_t fs_save_data_to_open_file_uint16(uint16_t openFileIndex, uint8_t index, uint16_t data, void* addr){
    if(index >= 2) return -2;

    uint32_t openFileOffset = sizeof(struct OpenFile) * openFileIndex;
    void* openFileTable_ptr = fs_get_open_file_table_ptr(addr);
    uint8_t offset = inner_fs_get_open_file_offsetof(index);
    uint8_t size = inner_fs_get_open_file_variable_size(index);

    if(offset == UINT8_MAX) return -1;

    memcpy(openFileTable_ptr + openFileOffset + offset, &data, size);

}

int8_t fs_save_data_to_open_file_uint32(uint16_t openFileIndex, uint8_t index, uint32_t data, void* addr){
    if(index <= 1) return -2;

    uint32_t openFileOffset = sizeof(struct OpenFile) * openFileIndex;
    void* openFileTable_ptr = fs_get_open_file_table_ptr(addr);
    uint8_t offset = inner_fs_get_open_file_offsetof(index);
    uint8_t size = inner_fs_get_open_file_variable_size(index);

    if(offset == UINT8_MAX) return -1;

    memcpy(openFileTable_ptr + openFileOffset + offset, &data, size);
}

int8_t fs_get_data_from_open_file_uint16(uint16_t openFileIndex, uint8_t index, uint16_t* data, void* addr){
    if(index >= 2) return -2;

    uint32_t openFileOffset = sizeof(struct OpenFile) * openFileIndex;
    void* openFileTable_ptr = fs_get_open_file_table_ptr(addr);
    uint8_t offset = inner_fs_get_open_file_offsetof(index);
    uint8_t size = inner_fs_get_open_file_variable_size(index);

    if(offset == UINT8_MAX) return -1;

    memcpy(data, openFileTable_ptr + openFileOffset + offset, size);

    return 0;
}

int8_t fs_get_data_from_open_file_uint32(uint16_t openFileIndex, uint8_t index, uint32_t* data, void* addr){
    if(index <= 1) return -2;

    uint32_t openFileOffset = sizeof(struct OpenFile) * openFileIndex;
    void* openFileTable_ptr = fs_get_open_file_table_ptr(addr);
    uint8_t offset = inner_fs_get_open_file_offsetof(index);
    uint8_t size = inner_fs_get_open_file_variable_size(index);

    if(offset == UINT8_MAX) return -1;

    memcpy(data, openFileTable_ptr + openFileOffset + offset, size);

    return 0;
}

int8_t fs_get_open_file_copy(uint16_t openFileIndex, struct OpenFile* openFileCopy, void* addr){
    void* addr_ptr = fs_get_open_file_table_ptr(addr) + (openFileIndex * sizeof(struct OpenFile));
    memcpy(openFileCopy, addr_ptr, sizeof(struct OpenFile));
    return 0;
}

int8_t fs_get_free_open_file(uint16_t* openFileIndex, void* addr){
    uint32_t ret = inner_fs_find_free_index(fs_get_open_file_bitmap_ptr(addr), fs_get_max_number_of_open_files(addr));

    if(ret == UINT32_MAX) return -1;
    if(ret == 0) return -2;

    *openFileIndex = ret;
    return 0;
}

int8_t fs_occupy_free_open_file(uint32_t* openFileIndex, struct OpenFile* openFileToSave, void* addr){
    void* openFileTable_ptr = fs_get_inode_table_ptr(addr);
    uint32_t ret = inner_fs_find_free_index(fs_get_open_file_bitmap_ptr(addr), fs_get_max_number_of_open_files(addr));

    if(ret == UINT32_MAX) return -1;
    if(ret == 0) return -2;

    *openFileIndex = ret;
    memcpy(openFileTable_ptr + (ret * sizeof(struct OpenFile)), openFileToSave, sizeof(struct OpenFile));
    fs_mark_open_file_as_used(ret, addr);

    return 0;
}

int8_t fs_create_open_file_table_stuctures_in_shm(void* addr){
    struct OpenFile toSave;
    uint32_t offset = 0;
    void* openFileTable_ptr = fs_get_open_file_table_ptr(addr);
    void* OpenFileStat_ptr = fs_get_inode_bitmap_ptr(addr);
    uint32_t maxNumberOfOpenFiles = fs_get_max_number_of_open_files(addr);
    uint32_t sizeofBitmapAlone = (maxNumberOfOpenFiles / 8) + 1;

    toSave.mode;
    toSave.inode_num;
    toSave.offset;

    for(unsigned int i = 0; i < maxNumberOfOpenFiles; ++i, offset += sizeof(struct OpenFile)){
        memcpy(openFileTable_ptr + offset, &toSave, sizeof(struct OpenFile));
    }


    struct OpenFileStat toSaveStat;
    toSaveStat.open_file_bitmap = malloc(sizeofBitmapAlone);

    toSaveStat.opened_files = 0;

    for(unsigned int i = 0; i < sizeofBitmapAlone; ++i){
        toSaveStat.open_file_bitmap[i] = 0xFF;
    }

    // last 8 bits
    int32_t modulo = maxNumberOfOpenFiles % 8;
    uint8_t lastBits = 0xFF;
    lastBits = lastBits << (8 - modulo);
    toSaveStat.open_file_bitmap[sizeofBitmapAlone - 1] = lastBits;

    memcpy(OpenFileStat_ptr, toSaveStat.open_file_bitmap, sizeofBitmapAlone);
    memcpy(OpenFileStat_ptr + sizeofBitmapAlone, &toSaveStat.opened_files, sizeof(uint16_t));

    free(toSaveStat.open_file_bitmap);
    return 0;
}

int8_t fs_mark_open_file_as_used(uint32_t openFileIndex, void* addr){
    return inner_fs_mark_bitmap_bit(fs_get_open_file_bitmap_ptr(addr), openFileIndex);
}

int8_t fs_mark_open_file_as_free(uint32_t openFileIndex, void* addr){
    return inner_fs_free_bitmap_bit(fs_get_open_file_bitmap_ptr(addr), openFileIndex);
}

#endif