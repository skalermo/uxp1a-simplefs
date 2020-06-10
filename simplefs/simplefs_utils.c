#include "simplefs_utils.h"
#include "open_files.h"
#include "inode.h"
#include "init.h"
#include "block_links.h"
#include <string.h>
#include <stdlib.h>

int parse_path(char* path, char*** subpath)
{
    int dirCount = 0;
    //path is invalid if path is NULL
    //or it is not absolute
    if(path == 0 || strncmp(path, "/", 1))
    {
        return EINPATH;
    }
    //counts approximate number of path's components
    for (int i = 0; path[i]; ++i)
    {
        dirCount += (path[i] == '/');
    }
    //allocates space for all names and root dir
    *subpath = (char** ) malloc((dirCount + 1) * sizeof(char*));

    int i = 0;
    char* tmp = strdup(path);
    char* token = strtok(tmp, "/");

    (*subpath)[i++] = strdup("/");
    while(token != NULL)
    {
        (*subpath)[i] = strdup(token);
        token = strtok(NULL, "/");
        ++i;
    }

    free(tmp);
    return i;
}

void free_string_array(char*** entries, int count)
{
    for(int i = 0; i < count; ++i )
    {
        free((*entries)[i]);
    }
    free(*entries);
    *entries = NULL;
}

int32_t next_inode(uint16_t prev_inode, char* name, uint8_t type, void* shm_addr){
    // if subpath is root return 1
    if(!strcmp(name, "/"))
        return 1;

    struct DirEntry copy;
    uint8_t inodeMode;

    // get dir file with its dir entries
    uint32_t dir_file_block = get_inode_block_index(prev_inode, shm_addr);

    uint32_t entry_idx = 2;

    // loop over all dir entries, break if arg name found in one of the entries
    while(fs_get_dir_entry_copy(dir_file_block, entry_idx, &copy, shm_addr) >= 0){
//        printf("%d %d %s\n", entry_idx, copy.inode_number, copy.name);
        if((copy.inode_number != 0) && !strcmp((char*) copy.name, name)){
            fs_get_data_from_inode_uint8(copy.inode_number, 4, &inodeMode, shm_addr);
            if((inodeMode & type) != 0)
                return (int32_t) copy.inode_number;
            return -2;
        }   
        ++entry_idx;
    }


    return -1;
}


int32_t get_inode_index(char *path, uint8_t type, void* shm_addr){
    int32_t current_inode = 1;

    char** sub_path = NULL;
    uint32_t sub_path_count = parse_path(path, &sub_path);

    if(sub_path_count < 0)
        return sub_path_count;

    long loopMax = sub_path_count - 1;
    for(int i = 0; i < loopMax; ++i){
        current_inode = next_inode(current_inode, sub_path[i], IS_DIR, shm_addr);
        if(current_inode < 0) {
            return -1;
        }
    }

    if(type == IS_DIR){
        struct ReadWriteSem semInode;
//        fs_sem_init_inode(&semInode, current_inode);
//        fs_sem_lock_read_inode(&semInode, shm_addr);
        current_inode = next_inode(current_inode, sub_path[loopMax], IS_DIR, shm_addr);
//        fs_sem_unlock_read_inode(&semInode, shm_addr);
//        fs_sem_close_inode(&semInode);
    }
    else if(type == IS_FILE){
        current_inode = next_inode(current_inode, sub_path[loopMax], IS_FILE, shm_addr);
    }
    else return -1;

    if(current_inode == -2) {
        return -2;
    }

    if(current_inode == -1) {
        return -1;
    }

    free_string_array(&sub_path, sub_path_count);

    return current_inode;
}

uint32_t allocate_new_chain(void* shm_addr){

    uint32_t block_num = fs_allocate_new_chain(shm_addr);

    return block_num;
}

int32_t save_new_inode(struct Inode* inode, void* shm_addr){
    uint16_t inode_idx;

    int8_t ret_value = fs_occupy_free_inode(&inode_idx, inode, shm_addr);

    if(ret_value < 0){
        return ret_value;
    }

    return inode_idx;
}

int32_t save_new_dir_entry(uint32_t dir_block_number, struct DirEntry* dir_entry, void* shm_addr){
    uint32_t entry_idx;

    int8_t ret_value = fs_occupy_free_dir_entry(dir_block_number, &entry_idx, dir_entry, shm_addr);

    if(ret_value < 0){
        return ret_value;
    }

    return entry_idx;
}

int32_t save_new_OpenFile_entry(struct OpenFile* open_file_entry, void* shm_addr){
    uint32_t open_file_idx;

    int8_t ret_value = fs_occupy_free_open_file(&open_file_idx, open_file_entry, shm_addr);

    if(ret_value < 0){
        return ret_value;
    }

    return open_file_idx;
}

struct Inode get_inode(uint16_t inode_idx, void* shm_addr){
    struct Inode inode = {0};

    fs_get_inode_copy(inode_idx, &inode, shm_addr);

    return inode;
}

uint32_t get_inode_block_index(uint16_t inode_idx, void* shm_addr){
    uint32_t block_idx;

    int8_t ret_value = fs_get_data_from_inode_uint32(inode_idx, 0, &block_idx, shm_addr);

    if(ret_value < 0){
        return ret_value;
    }

    return block_idx;
}

uint16_t get_inode_file_size(uint16_t inode_idx, void* shm_addr){
    uint16_t file_size;

    int8_t ret_value = fs_get_data_from_inode_uint16(inode_idx, 1, &file_size, shm_addr);

    if(ret_value < 0){
        return ret_value;
    }

    return file_size;
}

uint16_t get_inode_readers(uint16_t inode_idx, void* shm_addr){
    uint16_t readers;

    int8_t ret_value = fs_get_data_from_inode_uint16(inode_idx, 2, &readers, shm_addr);

    if(ret_value < 0){
        return ret_value;
    }

    return readers;
}

uint16_t get_inode_writers(uint16_t inode_idx, void* shm_addr){
    uint16_t writers;

    int8_t ret_value = fs_get_data_from_inode_uint16(inode_idx, 3, &writers, shm_addr);

    if(ret_value < 0){
        return ret_value;
    }

    return writers;
}

uint8_t  get_inode_mode(uint16_t inode_idx, void* shm_addr){
    uint8_t mode;

    int8_t ret_value = fs_get_data_from_inode_uint8(inode_idx, 4, &mode, shm_addr);

    if(ret_value < 0){
        return ret_value;
    }

    return mode;
}

uint8_t  get_ref_count(uint16_t inode_idx, void* shm_addr){
    uint8_t ref_count;

    int8_t ret_value = fs_get_data_from_inode_uint8(inode_idx, 5, &ref_count, shm_addr);

    if(ret_value < 0){
        return ret_value;
    }

    return ref_count;
}

int32_t read_buffer(uint32_t block_num, uint32_t offset, char* buf, int len, void* shm_addr) {

    // this function has to return number of bytes that were read
    // change in fs_get_data() required

    // target inode read semaphore
    fs_get_data(offset, offset + len, block_num, buf, shm_addr);
    return fs_get_data_count(offset, offset + len, block_num, buf, shm_addr);
}

int32_t write_buffer(uint32_t block_num, uint32_t offset, char* buf, int len, void* shm_addr) {

    // this function has to return number of bytes that were written
    // change in fs_save_data() required

    // target inode write semaphore
    // also block_stat semaphore here or inside fs_save_data 
    // because of block allocation
    fs_save_data(offset, offset + len, block_num, buf, shm_addr);
    return fs_save_data_count(offset, offset + len, block_num, buf, shm_addr);
}


int8_t set_inode_file_size(uint16_t inode, uint16_t filesize, void* shm_addr) {
    // can be changed only via simplefs_write
    // target inode write semaphore
    int8_t ret_value = fs_save_data_to_inode_uint16(inode, 1, filesize, shm_addr);
    return ret_value;
}

int8_t set_offset(uint16_t fd, uint32_t offset, void* shm_addr) {
    int8_t ret_value = fs_save_data_to_open_file_uint32(fd, 2, offset, shm_addr);
    return ret_value;
}

int16_t free_dir_entry(uint32_t dir_block_number, uint16_t inode, void *shm_addr) {
    struct DirEntry copy;
    uint32_t entry_idx = 0;
    int ret = -1;


    while(fs_get_dir_entry_copy(dir_block_number, entry_idx, &copy, shm_addr) >= 0){
        if(copy.inode_number == inode){
            // free dir entry
            ret = fs_save_data_to_dir_entry_inode_number(dir_block_number, entry_idx, 0, shm_addr);
            break;
        }
        ++entry_idx;
    }

    // If not found or error
    if(ret < 0)
        return -1;

    return 0;
}

int16_t free_inode(uint16_t inode, void *shm_addr) {
    int ret = fs_mark_inode_as_free(inode, shm_addr);
    if(ret < 0){
        return ret;
    }
    uint16_t used_inodes = fs_get_used_inodes(shm_addr);
    fs_set_used_inodes(used_inodes - 1, shm_addr);

    return 0;
}

int16_t free_data_blocks(uint32_t block_index, void *shm_addr) {
    int ret = fs_free_blockchain(block_index, shm_addr);
    if(ret < 0){
        return ret;
    }
    return 0;
}

struct OpenFile get_open_file(uint32_t fd, void *shm_addr) {
    struct OpenFile result;
    fs_get_open_file_copy(fd, &result, shm_addr);

    return result;
}

void inc_ref_count(uint16_t inode, void *shm_addr) {
    uint8_t ref_count;
    fs_get_data_from_inode_uint8(inode, 5, &ref_count, shm_addr);

    ref_count += 1;

    fs_save_data_to_inode_uint8(inode, 5, ref_count, shm_addr);
}

int is_dir_empty(uint16_t dir_inode, void *shm_addr) {
    uint32_t dir_block_number, next_block_number;
    fs_get_data_from_inode_uint32(dir_inode, 0, &dir_block_number, shm_addr);
    next_block_number = dir_block_number;

    void* block_ptr = fs_get_data_blocks_ptr(shm_addr) +
            dir_block_number * fs_get_data_block_size(shm_addr);
    do{
        struct DirEntry toRead;
        for (uint32_t i = 2; i < fs_get_data_block_size(shm_addr) / sizeof(struct DirEntry); ++i) {
            memcpy(&toRead, block_ptr + (i * sizeof(struct DirEntry)), sizeof(struct DirEntry));
            if(toRead.inode_number != 0){
                return 0;
            }
        }

        next_block_number = fs_get_next_block_number(next_block_number, shm_addr);
        block_ptr = fs_get_data_blocks_ptr(shm_addr) + next_block_number*fs_get_data_block_size(shm_addr);
    }
    while(next_block_number != FS_EMPTY_BLOCK_VALUE);

    return 1;
}