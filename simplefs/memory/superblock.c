/*
 * superblock.c
 *
 *      Author: Kordowski Mateusz
 */

#ifndef SIMPLEFS_SUPERBLOCK_C
#define SIMPLEFS_SUPERBLOCK_C

#include "superblock.h"
#include "utils.c"

///////////////////////////////////
//  Hidden functions
//////////////////////////////////

uint8_t inner_fs_get_superblock_offsetof(uint8_t index){
    switch(index){
        case 0:
            return offsetof(struct Superblock, max_number_of_inodes);
        case 1:
            return offsetof(struct Superblock, filesystem_checks);
        case 2:
            return offsetof(struct Superblock, data_block_size);
        case 3:
            return offsetof(struct Superblock, fs_size);
        case 4:
            return offsetof(struct Superblock, open_file_table_pointer);
        case 5:
            return offsetof(struct Superblock, open_file_bitmap_pointer);
        case 6:
            return offsetof(struct Superblock, block_links_pointer);
        case 7:
            return offsetof(struct Superblock, block_bitmap_pointer);
        case 8:
            return offsetof(struct Superblock, data_blocks_pointer);
        case 9:
            return offsetof(struct Superblock, inode_bitmap_pointer);
        case 10:
            return offsetof(struct Superblock, inode_table_pointer);
        default:
            return UINT8_MAX;
    }
}

uint8_t inner_fs_get_superblock_variable_size(uint8_t index){
    switch(index){
        case 0:
            return member_size(struct Superblock, max_number_of_inodes);
        case 1:
            return member_size(struct Superblock, filesystem_checks);
        case 2:
            return member_size(struct Superblock, data_block_size);
        case 3:
            return member_size(struct Superblock, fs_size);
        case 4:
            return member_size(struct Superblock, open_file_table_pointer);
        case 5:
            return member_size(struct Superblock, open_file_bitmap_pointer);
        case 6:
            return member_size(struct Superblock, block_links_pointer);
        case 7:
            return member_size(struct Superblock, block_bitmap_pointer);
        case 8:
            return member_size(struct Superblock, data_blocks_pointer);
        case 9:
            return member_size(struct Superblock, inode_bitmap_pointer);
        case 10:
            return member_size(struct Superblock, inode_table_pointer);
        default:
            return UINT8_MAX;
    }
}

void* innet_fs_get_superblock_pointers(void* addr, uint8_t index){
    void* ret;
    uint32_t ret_tmp;
    uint8_t offset = inner_fs_get_superblock_offsetof(index);
    uint8_t size = inner_fs_get_superblock_variable_size(index);

    memcpy(&ret_tmp, addr + offset, size);
    ret = ret_tmp + addr;

    return ret;
}

///////////////////////////////////
//  Struct functions
//////////////////////////////////

int8_t fs_get_from_superblock_uint32(uint8_t index, uint32_t* data, void* addr){
    #ifdef DEBUG
        if(index <= 2) return INT8_MIN;
    #endif

    uint8_t offset = inner_fs_get_superblock_offsetof(index);
    uint8_t size = inner_fs_get_superblock_variable_size(index);

    memcpy(data, addr + offset, size);

    return 0;
}

int8_t fs_get_data_from_superblock_uint16(uint8_t index, uint16_t* data, void* addr){
    #ifdef DEBUG
        if(index >=3) return INT8_MIN;
    #endif

    uint8_t offset = inner_fs_get_superblock_offsetof(index);
    uint8_t size = inner_fs_get_superblock_variable_size(index);

    memcpy(data, addr + offset, size);

    return 0;
}

int8_t fs_save_data_to_superblock_uint16(uint8_t index, uint16_t data, void* addr){
    #ifdef DEBUG
        if(index >=3) return INT8_MIN;
    #endif

    uint8_t offset = inner_fs_get_superblock_offsetof(index);
    uint8_t size = inner_fs_get_superblock_variable_size(index);

    memcpy(addr + offset, &data, size);

    return 0;
}

int8_t fs_save_data_to_superblock_uint32(uint8_t index, uint32_t data, void* addr){
    #ifdef DEBUG
        if(index <= 2) return INT8_MIN;
    #endif

    uint8_t offset = inner_fs_get_superblock_offsetof(index);
    uint8_t size = inner_fs_get_superblock_variable_size(index);

    memcpy(addr + offset, &data, size);

    return 0;
}

int8_t fs_get_superblock_copy(struct Superblock* superblockCopy, void* addr){
    memcpy(superblockCopy, addr, sizeof(struct Superblock));
    return 0;
}

void* fs_get_open_file_table_ptr(void* addr){
    return innet_fs_get_superblock_pointers(addr, 4);
}

void* fs_get_open_file_bitmap_ptr(void* addr){
    return innet_fs_get_superblock_pointers(addr, 5);
}

void* fs_get_block_links_ptr(void* addr){
    return innet_fs_get_superblock_pointers(addr, 6);
}

void* fs_get_block_bitmap_ptr(void* addr){
    return innet_fs_get_superblock_pointers(addr, 7);
}

void* fs_get_data_blocks_ptr(void* addr){
    return innet_fs_get_superblock_pointers(addr, 8);
}

void* fs_get_inode_table_ptr(void* addr){
    return innet_fs_get_superblock_pointers(addr, 9);
}

void* fs_get_inode_bitmap_ptr(void* addr){
    return innet_fs_get_superblock_pointers(addr, 10);
}

uint16_t fs_get_data_block_size(void* addr){
    uint32_t ret;
    uint8_t offset = inner_fs_get_superblock_offsetof(2);
    uint8_t size = inner_fs_get_superblock_variable_size(2);

    memcpy(&ret, addr + offset, size);

    return ret;
}

uint16_t fs_get_max_number_of_inodes(void* addr){
    uint32_t ret;
    uint8_t offset = inner_fs_get_superblock_offsetof(0);
    uint8_t size = inner_fs_get_superblock_variable_size(0);

    memcpy(&ret, addr + offset, size);

    return ret;
}


int8_t fs_create_superblock_in_shm(void* addr){
    struct Superblock toSave;
    
    toSave.max_number_of_inodes = FS_MAX_NUMBER_OF_INODES;
    toSave.filesystem_checks = 0;
    toSave.data_block_size = FS_BLOCK_SIZE;
    toSave.fs_size = FS_ENTIRE_SIZE;

    toSave.open_file_table_pointer = 44;
    toSave.open_file_bitmap_pointer;

    toSave.block_links_pointer = 798752;
    toSave.block_bitmap_pointer;

    toSave.inode_table_pointer = 12332;
    toSave.inode_bitmap_pointer;

    memcpy(addr, &toSave, sizeof(struct Superblock));

    return 0;
}

#endif