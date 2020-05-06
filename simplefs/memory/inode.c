/*
 * inode.c
 *
 *      Author: Kordowski Mateusz
 */

#ifndef SIMPLEFS_INODE_C
#define SIMPLEFS_INODE_C

#include "inode.h"
#include "utils.c"

///////////////////////////////////
//  Hidden functions
//////////////////////////////////

uint8_t inner_fs_get_inode_offsetof(uint8_t index){
    switch(index){
        case 0:
            return offsetof(struct Inode, block_index);
        case 1:
            return offsetof(struct Inode, file_size);
        case 2:
            return offsetof(struct Inode, mode);
        case 3:
            return offsetof(struct Inode, ref_count);
        case 4:
            return offsetof(struct Inode, readers);
        default:
            return UINT8_MAX;
    }
}

uint8_t inner_fs_get_inode_variable_size(uint8_t index){
    switch(index){
        case 0:
            return member_size(struct Inode, block_index);
        case 1:
            return member_size(struct Inode, file_size);
        case 2:
            return member_size(struct Inode, mode);
        case 3:
            return member_size(struct Inode, ref_count);
        case 4:
            return member_size(struct Inode, readers);
        default:
            return UINT8_MAX;
    }
}


///////////////////////////////////
//  Struct functions
//////////////////////////////////


int8_t fs_save_data_to_inode_uint8(uint16_t inodeIndex, uint8_t index, uint8_t data, void* addr){
    if(index <= 1) return -2;

    uint32_t inodeOffset = sizeof(struct Inode) * inodeIndex;
    void* inodeTable_ptr = fs_get_inode_table_ptr(addr);
    uint8_t offset = inner_fs_get_inode_offsetof(index);
    uint8_t size = inner_fs_get_inode_variable_size(index);

    if(offset == UINT8_MAX) return -1;

    memcpy(inodeTable_ptr + inodeOffset + offset, &data, size);

    return 0;
}

int8_t fs_save_data_to_inode_uint16(uint16_t inodeIndex, uint8_t index, uint16_t data, void* addr){
    if(index != 1) return -2;

    uint32_t inodeOffset = sizeof(struct Inode) * inodeIndex;
    void* inodeTable_ptr = fs_get_inode_table_ptr(addr);
    uint8_t offset = inner_fs_get_inode_offsetof(index);
    uint8_t size = inner_fs_get_inode_variable_size(index);

    if(offset == UINT8_MAX) return -1;

    memcpy(inodeTable_ptr + inodeOffset + offset, &data, size);

    return 0;
}

int8_t fs_save_data_to_inode_uint32(uint16_t inodeIndex, uint8_t index, uint32_t data, void* addr){
    if(index != 0) return -2;

    uint32_t inodeOffset = sizeof(struct Inode) * inodeIndex;
    void* inodeTable_ptr = fs_get_inode_table_ptr(addr);
    uint8_t offset = inner_fs_get_inode_offsetof(index);
    uint8_t size = inner_fs_get_inode_variable_size(index);

    if(offset == UINT8_MAX) return -1;

    memcpy(inodeTable_ptr + inodeOffset + offset, &data, size);

    return 0;
}

int8_t fs_get_data_from_inode_uint8(uint16_t inodeIndex, uint8_t index, uint8_t* data, void* addr){
    if(index <= 1) return -2;

    uint32_t inodeOffset = sizeof(struct Inode) * inodeIndex;
    void* inodeTable_ptr = fs_get_inode_table_ptr(addr);
    uint8_t offset = inner_fs_get_superblock_offsetof(index);
    uint8_t size = inner_fs_get_superblock_variable_size(index);

    if(offset == UINT8_MAX) return -1;

    memcpy(data, inodeTable_ptr + inodeOffset + offset, size);

    return 0;
}

int8_t fs_get_data_from_inode_uint16(uint16_t inodeIndex, uint16_t index, uint16_t* data, void* addr){
    if(index != 1) return -2;

    uint32_t inodeOffset = sizeof(struct Inode) * inodeIndex;
    void* inodeTable_ptr = fs_get_inode_table_ptr(addr);
    uint8_t offset = inner_fs_get_superblock_offsetof(index);
    uint8_t size = inner_fs_get_superblock_variable_size(index);

    if(offset == UINT8_MAX) return -1;

    memcpy(data, inodeTable_ptr + inodeOffset + offset, size);

    return 0;
}

int8_t fs_get_data_from_inode_uint32(uint16_t inodeIndex, uint32_t index, uint32_t* data, void* addr){
    if(index != 0) return -2;

    uint32_t inodeOffset = sizeof(struct Inode) * inodeIndex;
    void* inodeTable_ptr = fs_get_inode_table_ptr(addr);
    uint8_t offset = inner_fs_get_superblock_offsetof(index);
    uint8_t size = inner_fs_get_superblock_variable_size(index);

    if(offset == UINT8_MAX) return -1;

    memcpy(data, inodeTable_ptr + inodeOffset + offset, size);

    return 0;
}

int8_t fs_get_inode_copy(uint32_t inodeIndex, struct Inode* inodeCopy, void* addr){
    void* addr_ptr = fs_get_inode_table_ptr(addr) + (inodeIndex * sizeof(struct Inode));
    memcpy(inodeCopy, addr_ptr, sizeof(struct Inode));
    return 0;
}

int8_t fs_get_free_inode(uint32_t* inodeIndex, void* addr){
    uint32_t ret = inner_fs_find_free_index(fs_get_inode_bitmap_ptr(addr), FS_MAX_NUMBER_OF_INODES);

    if(ret == UINT32_MAX) return -1;
    if(ret == 0) return -2;

    *inodeIndex = ret;
    return 0;
}

int8_t fs_occupy_free_inode(uint32_t* inodeIndex, struct Inode* inodeToSave, void* addr){
    void* inodeTable_ptr = fs_get_inode_table_ptr(addr);
    uint32_t ret = inner_fs_find_free_index(fs_get_inode_bitmap_ptr(addr), FS_MAX_NUMBER_OF_INODES);

    if(ret == UINT32_MAX) return -1;
    if(ret == 0) return -2;

    inner_fs_mark_bitmap_bit(fs_get_inode_bitmap_ptr(addr), ret);

    *inodeIndex = ret;
    memcpy(inodeTable_ptr + (ret * sizeof(struct Inode)), inodeToSave, sizeof(struct Inode));
    return 0;
}

int8_t fs_mark_inode_as_used(uint32_t inodeIndex, void* addr){
    return inner_fs_mark_bitmap_bit(fs_get_inode_bitmap_ptr(addr), inodeIndex);
}

int8_t fs_mark_inode_as_free(uint32_t inodeIndex, void* addr){
    return inner_fs_free_bitmap_bit(fs_get_inode_bitmap_ptr(addr), inodeIndex);
}

int8_t fs_create_inode_structures_in_shm(void* addr){
    struct Inode toSave;
    void* inodeTable_ptr = fs_get_inode_table_ptr(addr);
    void* inoteStat_ptr = fs_get_inode_bitmap_ptr(addr);
    uint32_t offset = 0;

    // save inode table

    // main directory
    toSave.block_index;
    toSave.file_size;
    toSave.mode;
    toSave.ref_count = 0;
    toSave.readers = 0;
    toSave.padding[0] = 0;
    toSave.padding[1] = 0;
    toSave.padding[2] = 0;

    memcpy(inodeTable_ptr + offset, &toSave, sizeof(struct Inode));
    offset += sizeof(struct Inode);

    // rest of inodes
    toSave.block_index;
    toSave.file_size;
    toSave.mode;
    
    for(unsigned int i = 1; i < FS_MAX_NUMBER_OF_INODES; ++i, offset += sizeof(struct Inode)){
        memcpy(inodeTable_ptr + offset, &toSave, sizeof(struct Inode));
    }

    struct InodeStat* toSaveStat = malloc(sizeof(struct InodeStat));

    toSaveStat->inode_used = 1;
    toSaveStat->inode_bitmap[0] = 0x7F;

    for(unsigned int i = 1; i < FS_MAX_NUMBER_OF_INODES_BY_8; ++i){
        toSaveStat->inode_bitmap[i] = 1;
    }

    memcpy(inoteStat_ptr, toSaveStat, sizeof(struct InodeStat));

    free(toSaveStat);
    return 0;
}

#endif