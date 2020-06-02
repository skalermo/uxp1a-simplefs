/*
 * dir_file.c
 *
 *      Author: Kordowski Mateusz
 */


#include "dir_file.h"


///////////////////////////////////
//  Hidden functions
//////////////////////////////////

/**
 * @brief Creates in this block an table of DirEntry structures with default values
 * 
 * @param blockNumber - index of a block where the structures will be created
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful
 * No other errors.
 */
int8_t inner_fs_create_empty_dir_file_struct_in_block(uint32_t* blockNumber, void* addr){

    struct DirEntry* toSave = malloc(sizeof(struct DirEntry));
    void* block_ptr = fs_get_data_blocks_ptr(addr) + (*blockNumber * fs_get_data_block_size(addr));

    toSave->inode_number = 0;
    for(unsigned int i = 0; i < FS_NAME_SIZE; ++i){
        toSave->name[i] = 0;
    }
    toSave->name_len = 0;

    for(uint16_t i = 0; i < fs_get_data_block_size(addr) / sizeof(struct DirEntry); i += sizeof(struct DirEntry)) {
        memcpy(block_ptr + i, toSave, sizeof(struct DirEntry));
    }

    free(toSave);
    return 0;
}

/**
 * @brief 
 * 
 * @param blockNumber - pointer that stores value of the first block in blockchain.
 * At return it stores the index of a block where the search index is found.
 * It does not allocate any more blocks. It only search through already allocated blocks.
 * 
 * @param dirEntryIndex - pointer that stores value of the directory entry index.
 * At return it stores the index of an entry directory in relation to returned block index.
 *  
 * @param addr - address of the mapped shared memory.
 * @return int8_t - 0 if operation was successful.
 * -1 if the directory entry index goes beyond allocated memmory (blocks).
 */
int8_t inner_fs_find_block_through_index_with_error(uint32_t* blockNumber, uint32_t* dirEntryIndex, void* addr){
    uint32_t freeEntryIndexInBlock = *dirEntryIndex;
    uint32_t realBlockNumber = *blockNumber;

    if(freeEntryIndexInBlock * sizeof(struct DirEntry) > fs_get_data_block_size(addr)){
        // find block where this entry index is
        uint32_t howManyBlocks = (freeEntryIndexInBlock * sizeof(struct DirEntry)) / fs_get_data_block_size(addr);
        uint32_t indexDifference = fs_get_data_block_size(addr) / sizeof(struct DirEntry);

        for(uint32_t i = 0; i < howManyBlocks; ++i){
            int8_t ret = inner_fs_next_block_with_error(&realBlockNumber, addr);
            if(ret == -1) return -1;
            freeEntryIndexInBlock -= indexDifference;
        }
    }
    *blockNumber = realBlockNumber;
    *dirEntryIndex = freeEntryIndexInBlock;

    return 0;
}

///////////////////////////////////
//  Struct functions
//////////////////////////////////

int8_t fs_save_data_to_dir_entry_name(uint32_t blockNumber, uint32_t dirEntryIndex, char* name, uint8_t nameSize, void* addr){
    if(nameSize > FS_NAME_SIZE) return -1;

    void* block_ptr;
    uint32_t freeEntryIndexInBlock = dirEntryIndex;
    uint32_t realBlockNumber = blockNumber;

    if(inner_fs_find_block_through_index_with_error(&realBlockNumber, &freeEntryIndexInBlock, addr) == -1) return -1;

    // save new data
    block_ptr = fs_get_data_blocks_ptr(addr) + (realBlockNumber * fs_get_data_block_size(addr));
    uint32_t offsetInBlock = freeEntryIndexInBlock * sizeof(struct DirEntry);
    memcpy(block_ptr + offsetInBlock, name, nameSize);
    memcpy(block_ptr + offsetInBlock + (FS_NAME_SIZE * sizeof(uint8_t)), &nameSize, sizeof(uint8_t));

    return 0;
}

int8_t fs_save_data_to_dir_entry_inode_number(uint32_t blockNumber, uint32_t dirEntryIndex, uint16_t inodeNumber, void* addr){
    void* block_ptr;
    uint32_t freeEntryIndexInBlock = dirEntryIndex;
    uint32_t realBlockNumber = blockNumber;

    if(inner_fs_find_block_through_index_with_error(&realBlockNumber, &freeEntryIndexInBlock, addr) == -1) return -1;

    // save new data
    block_ptr = fs_get_data_blocks_ptr(addr) + (realBlockNumber * fs_get_data_block_size(addr));
    uint32_t offsetInBlock = freeEntryIndexInBlock * sizeof(struct DirEntry);
    memcpy(block_ptr + offsetInBlock + (FS_NAME_SIZE * sizeof(uint8_t)) + sizeof(uint8_t), &inodeNumber, sizeof(uint16_t));

    return 0;
}

int8_t fs_get_data_from_dir_entry_name(uint32_t blockNumber, uint32_t dirEntryIndex, char* name, uint8_t* nameSize, void* addr){
    void* block_ptr;
    uint32_t freeEntryIndexInBlock = dirEntryIndex;
    uint32_t realBlockNumber = blockNumber;

    if(inner_fs_find_block_through_index_with_error(&realBlockNumber, &freeEntryIndexInBlock, addr) == -1) return -1;

    block_ptr = fs_get_data_blocks_ptr(addr) + (realBlockNumber * fs_get_data_block_size(addr));
    uint32_t offsetInBlock = freeEntryIndexInBlock * sizeof(struct DirEntry);

    memcpy(nameSize, block_ptr + offsetInBlock + (FS_NAME_SIZE * sizeof(uint8_t)), sizeof(uint8_t));
    memcpy(name, block_ptr + offsetInBlock, *nameSize);

    return 0;
}

int8_t fs_get_data_from_dir_entry_inode_number(uint32_t blockNumber, uint32_t dirEntryIndex, uint16_t* inodeNumber, void* addr){
    void* block_ptr;
    uint32_t freeEntryIndexInBlock = dirEntryIndex;
    uint32_t realBlockNumber = blockNumber;

    if(inner_fs_find_block_through_index_with_error(&realBlockNumber, &freeEntryIndexInBlock, addr) == -1) return -1;

    block_ptr = fs_get_data_blocks_ptr(addr) + (realBlockNumber * fs_get_data_block_size(addr));
    uint32_t offsetInBlock = freeEntryIndexInBlock * sizeof(struct DirEntry);

    memcpy(inodeNumber, block_ptr + offsetInBlock + (FS_NAME_SIZE * sizeof(uint8_t)) + sizeof(uint8_t), sizeof(uint16_t));

    return 0;
}

int8_t fs_get_dir_entry_copy(uint32_t blockNumber, uint32_t dirEntryIndex, struct DirEntry* dirEntryCopy, void* addr){
    void* block_ptr;
    uint32_t freeEntryIndexInBlock = dirEntryIndex;
    uint32_t realBlockNumber = blockNumber;

    if(inner_fs_find_block_through_index_with_error(&realBlockNumber, &freeEntryIndexInBlock, addr) == -1) return -1;

    block_ptr = fs_get_data_blocks_ptr(addr) + (realBlockNumber * fs_get_data_block_size(addr));
    int inOneBlock = fs_get_data_block_size(addr) / sizeof(struct DirEntry);
    freeEntryIndexInBlock = freeEntryIndexInBlock % inOneBlock;
    uint32_t offsetInBlock = freeEntryIndexInBlock * sizeof(struct DirEntry);

    memcpy(dirEntryCopy, block_ptr + offsetInBlock, sizeof(struct DirEntry));

    return 0;
}

int8_t fs_get_dir_file_copy(uint32_t blockNumber, struct DirFile* dirFileCopy, uint32_t* sizeofDirFileCopy, void* addr){
    void* block_ptr;
    uint32_t dirFileCopyOffset = 0;
    uint32_t nextBlockNumber = blockNumber;
    uint32_t previousBlockNumber = blockNumber;
    uint32_t sizeofEntriesInOneBlock = (fs_get_data_block_size(addr) / sizeof(struct DirEntry)) * sizeof(struct DirEntry);

    uint32_t blockIndexCheck = blockNumber;
    uint32_t sizeofDirFile = 0;

    do{
        sizeofDirFile += sizeofEntriesInOneBlock;
        blockIndexCheck = fs_get_next_block_number(blockIndexCheck, addr);
    }
    while(blockIndexCheck != FS_EMPTY_BLOCK_VALUE);

    // allocate memory
    dirFileCopy->entry = malloc(sizeofDirFile);
    *sizeofDirFileCopy = sizeofDirFile;

    do{
        previousBlockNumber = nextBlockNumber;
        block_ptr = fs_get_data_blocks_ptr(addr) + (previousBlockNumber * fs_get_data_block_size(addr));

        memcpy(dirFileCopy->entry + dirFileCopyOffset, block_ptr, sizeofEntriesInOneBlock);
        dirFileCopyOffset += sizeofEntriesInOneBlock;

        nextBlockNumber = fs_get_next_block_number(previousBlockNumber, addr);
    }
    while(nextBlockNumber != FS_EMPTY_BLOCK_VALUE);

    return 0;
}

int8_t fs_create_dir_file(uint32_t* blockNumber, struct FS_create_dir_data* inodesData, void* addr){
    uint32_t retBlockIndex = fs_allocate_new_chain(addr);
    if(retBlockIndex == FS_EMPTY_BLOCK_VALUE) return -1;

    struct DirEntry* toSave = malloc(sizeof(struct DirEntry));
    void* block_ptr = fs_get_data_blocks_ptr(addr) + (retBlockIndex * fs_get_data_block_size(addr));

    // directory '.'
    toSave->inode_number = inodesData->thisDirInode;
    memcpy(toSave->name, inodesData->thisDirName, inodesData->thisDirNameLen);
    toSave->name_len = inodesData->thisDirNameLen;
    memcpy(block_ptr, toSave, sizeof(struct DirEntry));

    // directory '..'
    toSave->inode_number = inodesData->prevoiusDirInode;
    memcpy(toSave->name, inodesData->prevoiusDirInodeName, inodesData->prevoiusDirInodeLen);
    toSave->name_len = inodesData->prevoiusDirInodeLen;
    memcpy(block_ptr + sizeof(struct DirEntry), toSave, sizeof(struct DirEntry));

    // rest of block
    toSave->inode_number = 0;
    for(unsigned int i = 0; i < FS_NAME_SIZE; ++i){
        toSave->name[i] = 0;
    }
    toSave->name_len = 0;

    for(int i = 2; i < fs_get_data_block_size(addr) / sizeof(struct DirEntry); i++) {
        memcpy(block_ptr + i * sizeof(struct DirEntry), toSave, sizeof(struct DirEntry));
    }

    free(toSave);
    *blockNumber = retBlockIndex;
    return 0;
}

int8_t fs_create_and_save_dir_file(uint32_t* blockNumber, struct FS_create_dir_data* inodesData, struct DirEntry* dirEntryToSave, void* addr){
    
    if(fs_create_dir_file(blockNumber, inodesData, addr) == -1) return -1;
    void* block_ptr = fs_get_data_blocks_ptr(addr) + (*blockNumber * fs_get_data_block_size(addr));
    memcpy(block_ptr + (sizeof(struct DirEntry) * 2), dirEntryToSave, sizeof(struct DirEntry));
    return 0;
}

int8_t fs_create_main_folder(void* addr){
    const uint32_t blockindex = 0;
    struct DirEntry* toSave = malloc(sizeof(struct DirEntry));
    void* block_ptr = fs_get_data_blocks_ptr(addr) + (blockindex * fs_get_data_block_size(addr));

    // directory '.'
    strcpy(toSave->name, FS_MAIN_DIRECTORY_NAME);
    toSave->name_len = FS_MAIN_DIRECTORY_NAME_SIZE;
    toSave->inode_number = 1;

    memcpy(block_ptr, toSave, sizeof(struct DirEntry));

    // directory '.' again, for compatibility
    memcpy(block_ptr + sizeof(struct DirEntry), toSave, sizeof(struct DirEntry));

    toSave->inode_number = 0;
    for(unsigned int i = 0; i < FS_NAME_SIZE; ++i){
        toSave->name[i] = 0;
    }
    toSave->name_len = 0;

    for(uint16_t i = sizeof(struct DirEntry) * 2; i < fs_get_data_block_size(addr) / sizeof(struct DirEntry); i += sizeof(struct DirEntry)) {
        memcpy(block_ptr + i, toSave, sizeof(struct DirEntry));
    }

    free(toSave);
    return 0;
}

int8_t fs_get_free_dir_entry(uint32_t blockNumber, uint32_t* dirEntryIndex, void* addr){
    void* block_ptr = fs_get_data_blocks_ptr(addr) + (blockNumber * sizeof(struct DirEntry));
    struct DirEntry toRead;
    uint32_t nextBlockIndex = blockNumber;

    do{
        for(uint32_t i = 0; i < fs_get_data_block_size(addr) / sizeof(struct DirEntry); ++i){
            memcpy(&toRead, block_ptr + (i * sizeof(struct DirEntry)), sizeof(struct DirEntry));
            if(toRead.inode_number == 0){
                // found
                *dirEntryIndex = i;
                return 0;
            }
        }
        int8_t retVal = inner_fs_next_block_with_allocate(&nextBlockIndex, addr);
        if(retVal == -1) return -1;
        if(retVal == 1){ // new block was allocated
            // create empty structures
            inner_fs_create_empty_dir_file_struct_in_block(&nextBlockIndex, addr);
        }

        block_ptr = fs_get_data_blocks_ptr(addr) + (nextBlockIndex * sizeof(struct DirEntry));
    }
    while(1); // either it must return success or fail.
    
    return -1; // unacceptable state
}

int8_t fs_occupy_free_dir_entry(uint32_t blockNumber, uint32_t* dirEntryIndex, struct DirEntry* dirEntryToSave, void* addr){
    void* block_ptr = fs_get_data_blocks_ptr(addr) + (blockNumber * fs_get_data_block_size(addr));
    struct DirEntry toRead;
    uint32_t nextBlockIndex = blockNumber;

    do{
        for(uint32_t i = 0; i < fs_get_data_block_size(addr) / sizeof(struct DirEntry); ++i){
            memcpy(&toRead, block_ptr + (i * sizeof(struct DirEntry)), sizeof(struct DirEntry));
            if(toRead.inode_number == 0){
                // found
                *dirEntryIndex = i;
                memcpy(block_ptr + (i * sizeof(struct DirEntry)), dirEntryToSave, sizeof(struct DirEntry));
                return 0;
            }
        }
        int8_t retVal = inner_fs_next_block_with_allocate(&nextBlockIndex, addr);
        if(retVal == -1) return -1;
        if(retVal == 1){ // new block was allocated
            // create empty structures
            inner_fs_create_empty_dir_file_struct_in_block(&nextBlockIndex, addr);
        }

        block_ptr = fs_get_data_blocks_ptr(addr) + (nextBlockIndex * sizeof(struct DirEntry));
    }
    while(1); // either it must return success or fail.
    
    return -1; // unacceptable state
}
