/*
 * superblock.c
 *
 *      Author: Kordowski Mateusz
 */


#include "superblock.h"

///////////////////////////////////
//  Hidden functions
//////////////////////////////////

uint8_t inner_fs_get_superblock_offsetof(uint8_t index){
    switch(index){
        case 0:
            return offsetof(struct Superblock, max_number_of_inodes);
        case 1:
            return offsetof(struct Superblock, max_number_of_open_files);
        case 2:
            return offsetof(struct Superblock, filesystem_checks);
        case 3:
            return offsetof(struct Superblock, data_block_size);
        case 4:
            return offsetof(struct Superblock, number_of_data_blocks);
        case 5:
            return offsetof(struct Superblock, fs_size);
        case 6:
            return offsetof(struct Superblock, open_file_table_pointer);
        case 7:
            return offsetof(struct Superblock, open_file_bitmap_pointer);
        case 8:
            return offsetof(struct Superblock, inode_table_pointer);
        case 9:
            return offsetof(struct Superblock, inode_bitmap_pointer);
        case 10:
            return offsetof(struct Superblock, block_links_pointer);
        case 11:
            return offsetof(struct Superblock, block_bitmap_pointer);
        case 12:
            return offsetof(struct Superblock, data_blocks_pointer);
        default:
            return UINT8_MAX;
    }
}

uint8_t inner_fs_get_superblock_variable_size(uint8_t index){
    switch(index){
        case 0:
            return member_size(struct Superblock, max_number_of_inodes);
        case 1:
            return member_size(struct Superblock, max_number_of_open_files);
        case 2:
            return member_size(struct Superblock, filesystem_checks);
        case 3:
            return member_size(struct Superblock, data_block_size);
        case 4:
            return member_size(struct Superblock, number_of_data_blocks);
        case 5:
            return member_size(struct Superblock, fs_size);
        case 6:
            return member_size(struct Superblock, open_file_table_pointer);
        case 7:
            return member_size(struct Superblock, open_file_bitmap_pointer);
        case 8:
            return member_size(struct Superblock, inode_table_pointer);
        case 9:
            return member_size(struct Superblock, inode_bitmap_pointer);
        case 10:
            return member_size(struct Superblock, block_links_pointer);
        case 11:
            return member_size(struct Superblock, block_bitmap_pointer);
        case 12:
            return member_size(struct Superblock, data_blocks_pointer);
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

uint16_t inner_fs_get_superblock_uint16(void* addr, uint8_t index){
    uint16_t ret;
    uint8_t offset = inner_fs_get_superblock_offsetof(index);
    uint8_t size = inner_fs_get_superblock_variable_size(index);

    memcpy(&ret, addr + offset, size);

    return ret;
}

///////////////////////////////////
//  Struct functions
//////////////////////////////////

int8_t fs_get_data_from_superblock_uint32(uint8_t index, uint32_t* data, void* addr){
    if(index <= 3) return -2;

    uint8_t offset = inner_fs_get_superblock_offsetof(index);
    uint8_t size = inner_fs_get_superblock_variable_size(index);

    if(offset == UINT8_MAX) return -1;

    memcpy(data, addr + offset, size);

    return 0;
}

int8_t fs_get_data_from_superblock_uint16(uint8_t index, uint16_t* data, void* addr){
        if(index >=4) return -2;

    uint8_t offset = inner_fs_get_superblock_offsetof(index);
    uint8_t size = inner_fs_get_superblock_variable_size(index);

    if(offset == UINT8_MAX) return -1;

    memcpy(data, addr + offset, size);

    return 0;
}

int8_t fs_save_data_to_superblock_uint16(uint8_t index, uint16_t data, void* addr){
    if(index >=4) return -2;

    uint8_t offset = inner_fs_get_superblock_offsetof(index);
    uint8_t size = inner_fs_get_superblock_variable_size(index);

    if(offset == UINT8_MAX) return -1;

    memcpy(addr + offset, &data, size);

    return 0;
}

int8_t fs_save_data_to_superblock_uint32(uint8_t index, uint32_t data, void* addr){
    if(index <= 3) return -2;

    uint8_t offset = inner_fs_get_superblock_offsetof(index);
    uint8_t size = inner_fs_get_superblock_variable_size(index);

    if(offset == UINT8_MAX) return -1;

    memcpy(addr + offset, &data, size);

    return 0;
}

int8_t fs_get_superblock_copy(struct Superblock* superblockCopy, void* addr){
    memcpy(superblockCopy, addr, sizeof(struct Superblock));
    return 0;
}

void* fs_get_open_file_table_ptr(void* addr){
    return innet_fs_get_superblock_pointers(addr, 6);
}

void* fs_get_open_file_bitmap_ptr(void* addr){
    return innet_fs_get_superblock_pointers(addr, 7);
}

void* fs_get_block_links_ptr(void* addr){
    return innet_fs_get_superblock_pointers(addr, 10);
}

void* fs_get_block_bitmap_ptr(void* addr){
    return innet_fs_get_superblock_pointers(addr, 11);
}

void* fs_get_data_blocks_ptr(void* addr){
    return innet_fs_get_superblock_pointers(addr, 12);
}

void* fs_get_inode_table_ptr(void* addr){
    return innet_fs_get_superblock_pointers(addr, 8);
}

void* fs_get_inode_bitmap_ptr(void* addr){
    return innet_fs_get_superblock_pointers(addr, 9);
}

uint16_t fs_get_data_block_size(void* addr){
    return inner_fs_get_superblock_uint16(addr, 3);
}

uint16_t fs_get_max_number_of_inodes(void* addr){
    return inner_fs_get_superblock_uint16(addr, 0);
}

uint16_t fs_get_max_number_of_open_files(void* addr){
    return inner_fs_get_superblock_uint16(addr, 1);
}

uint32_t fs_get_max_number_data_blocks(void* addr){
    uint32_t ret;
    uint8_t offset = inner_fs_get_superblock_offsetof(4);
    uint8_t size = inner_fs_get_superblock_variable_size(4);

    memcpy(&ret, addr + offset, size);

    return ret;
}

int8_t fs_create_superblock_in_shm(struct Superblock* toCopy, void* addr){
    memcpy(addr, toCopy, sizeof(struct Superblock));

    return 0;
}

/////////////////////////////////////
//  Functions to calculate offsets
////////////////////////////////////

uint32_t calculate_fs_superblock_end(){
    return sizeof(struct Superblock);
}

uint32_t calculate_fs_open_file_table_end(uint32_t maxOpenFiles, uint32_t maxInodes, uint32_t maxFilesystemSize, uint32_t sizeofOneBlock){
    return sizeof(struct OpenFile) * maxOpenFiles + calculate_fs_superblock_end();
}

uint32_t calculate_fs_open_file_stat_end(uint32_t maxOpenFiles, uint32_t maxInodes, uint32_t maxFilesystemSize, uint32_t sizeofOneBlock){
    uint32_t fun = calculate_fs_open_file_table_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock);
    uint32_t sizeOfStat = (maxOpenFiles / 8) + 1 + 2;
    return fun + sizeOfStat;
}

uint32_t calculate_fs_inode_table_end(uint32_t maxOpenFiles, uint32_t maxInodes, uint32_t maxFilesystemSize, uint32_t sizeofOneBlock){
    uint32_t fun = calculate_fs_open_file_stat_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock);
    return sizeof(struct Inode) * maxInodes + fun;
}

uint32_t calculate_fs_inode_stat_end(uint32_t maxOpenFiles, uint32_t maxInodes, uint32_t maxFilesystemSize, uint32_t sizeofOneBlock){
    uint32_t fun = calculate_fs_inode_table_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock);
    uint32_t sizeOfStat = (maxInodes / 8) + 1 + 2;
    return fun + sizeOfStat;
}

uint32_t calculate_fs_block_links_end(uint32_t maxOpenFiles, uint32_t maxInodes, uint32_t maxFilesystemSize, uint32_t sizeofOneBlock){
    uint32_t fun = calculate_fs_inode_stat_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock);
    uint32_t neededBlocks = calculate_fs_needed_blocks(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock);
    return neededBlocks * sizeof(uint32_t) + fun;
}

uint32_t calculate_fs_block_stat_end(uint32_t maxOpenFiles, uint32_t maxInodes, uint32_t maxFilesystemSize, uint32_t sizeofOneBlock){
    uint32_t fun = calculate_fs_block_links_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock);
    uint32_t neededBlocks = calculate_fs_needed_blocks(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock);
    uint32_t sizeOfStat = (neededBlocks / 8) + 1 + 4;
    return fun + sizeOfStat;
}

uint32_t calculate_fs_data_block_end(uint32_t maxOpenFiles, uint32_t maxInodes, uint32_t maxFilesystemSize, uint32_t sizeofOneBlock){
    uint32_t fun = calculate_fs_block_stat_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock);
    uint32_t neededBlocks = calculate_fs_needed_blocks(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock);
    return neededBlocks * sizeofOneBlock + fun;
}

uint32_t calculate_fs_needed_blocks(uint32_t maxOpenFiles, uint32_t maxInodes, uint32_t maxFilesystemSize, uint32_t sizeofOneBlock){
    uint32_t fun = calculate_fs_inode_stat_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock);
    return (maxFilesystemSize - 1 - fun) * 8 / ((8 * (sizeof(uint32_t) + sizeofOneBlock)) + 1);  // floor
}
