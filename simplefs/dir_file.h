#ifndef DIR_FILE_H
#define DIR_FILE_H

#include <stdint.h>
#include <stddef.h> // offsetof
#include "inode.h"
#include "block_links.h"

///////////////////////////////////
//  Structs
//////////////////////////////////

struct DirEntry{
    uint8_t name[61];
    uint8_t name_len;
    uint16_t inode_number;
};

struct DirFile{
    struct DirEntry* entry; // we dont know how many blocks will be allocated.
};


///////////////////////////////////
//  Struct functions
//////////////////////////////////


int fs_save_data_to_dir_entry_name(uint32_t blockNumber, uint32_t dirEntryIndex, char* name);

int fs_save_data_to_dir_entry_inode_number(uint32_t blockNumber, uint32_t dirEntryIndex, uint16_t inodeNumber);


int fs_get_data_from_dir_entry_name(uint32_t blockNumber, uint32_t dirEntryIndex, char* name);

int fs_get_data_from_dir_entry_inode_number(uint32_t blockNumber, uint32_t dirEntryIndex, uint16_t* inodeNumber);


int fs_get_dir_entry_copy(uint32_t blockNumber, uint32_t dirEntryIndex, DirEntry* dirFileCopy);

int fs_get_dir_file_copy(uint32_t blockNumber, DirFile* dirFileCopy);


int fs_create_dir_file(uint32_t* blockNumber);


int fs_get_free_dir_entry(uint32_t blockNumber, uint32_t* dirEntryIndex);




#endif