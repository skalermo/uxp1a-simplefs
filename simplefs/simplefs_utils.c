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

/**
 * @brief Find free bit
 * @param bitmap
 * @param size
 * @return index or -1 if not found
 */
int find_free_bit(uint8_t *bitmap, int size){
    // skip all zeros
    for(int i = 0; i < size; ++i){
        if(bitmap[i] != 0){
            // count leading zeros + 1
            int first_bit = __builtin_clz(bitmap[i]) - 23; // 23 = 16 + 8 - 1

            return 8 * i + first_bit - 1; // 8 = uint8_t
        }
    }
    return -1;
}

/**
 * @brief Set bit in bitmap
 * @param bitmap
 * @param index
 */
void set_bit(uint8_t *bitmap, int index){
    int bitmap_idx = index / 8; // 8 = uint8_t
    int idx = index % 8;

    uint8_t one_hot = 0b10000000;
    for(int i = 0; i < idx; ++i){ one_hot >>=1;} // One Hot encode idx

    bitmap[bitmap_idx] |= one_hot;
}

/**
 * @brief Unset bit in bitmap
 * @param bitmap
 * @param index
 */
void unset_bit(uint8_t *bitmap, int index){
    int bitmap_idx = index / 8; // 8 = uint8_t
    int idx = index % 8;

    uint8_t one_hot = 0b10000000;
    for(int i = 0; i < idx; ++i){ one_hot >>=1;} // One Hot encode idx

    bitmap[bitmap_idx] &= ~one_hot;
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

        if(copy.inode_number != 0 && !strcmp((char*) copy.name, name)){
            fs_get_data_from_inode_uint8(copy.inode_number, 4, &inodeMode, shm_addr);
            if((inodeMode & type) != 0)
                return copy.inode_number;
            return -2;
        }   
        ++entry_idx;
    }
    // signal

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
        fs_sem_init_inode(&semInode, current_inode);
        fs_sem_lock_read_inode(&semInode, shm_addr);
        current_inode = next_inode(current_inode, sub_path[loopMax], IS_DIR, shm_addr);
        fs_sem_unlock_read_inode(&semInode, shm_addr);
        fs_sem_close_inode(&semInode);
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
    // wait
    uint32_t block_num = fs_allocate_new_chain(shm_addr);
    // signal

    return block_num;
}

int32_t save_new_inode(struct Inode* inode, void* shm_addr){
    uint16_t inode_idx;

    // wait
    int8_t ret_value = fs_occupy_free_inode(&inode_idx, inode, shm_addr);
    // signal

    if(ret_value < 0){
        return ret_value;
    }

    return inode_idx;
}

int32_t save_new_dir_entry(uint32_t dir_block_number, struct DirEntry* dir_entry, void* shm_addr){
    uint32_t entry_idx;

    // wait
    int8_t ret_value = fs_occupy_free_dir_entry(dir_block_number, &entry_idx, dir_entry, shm_addr);
    // signal

    if(ret_value < 0){
        return ret_value;
    }

    return entry_idx;
}

int32_t save_new_OpenFile_entry(struct OpenFile* open_file_entry, void* shm_addr){
    uint32_t open_file_idx;

    // wait
    int8_t ret_value = fs_occupy_free_open_file(&open_file_idx, open_file_entry, shm_addr);
    // signal

    if(ret_value < 0){
        return ret_value;
    }

    return open_file_idx;
}


// Inode getters
struct Inode get_inode(uint16_t inode_idx, void* shm_addr){
    struct Inode inode = {0};

    // wait
    fs_get_inode_copy(inode_idx, &inode, shm_addr);
    // signal

    return inode;
}

uint32_t get_inode_block_index(uint16_t inode_idx, void* shm_addr){
    uint32_t block_idx;

    // wait
    int8_t ret_value = fs_get_data_from_inode_uint32(inode_idx, 0, &block_idx, shm_addr);
    // signal

    if(ret_value < 0){
        return ret_value;
    }

    return block_idx;
}

uint16_t get_inode_file_size(uint16_t inode_idx, void* shm_addr){
    uint16_t file_size;

    // wait
    int8_t ret_value = fs_get_data_from_inode_uint16(inode_idx, 1, &file_size, shm_addr);
    // signal

    if(ret_value < 0){
        return ret_value;
    }

    return file_size;
}

uint16_t get_inode_readers(uint16_t inode_idx, void* shm_addr){
    uint16_t readers;

    // wait
    int8_t ret_value = fs_get_data_from_inode_uint16(inode_idx, 2, &readers, shm_addr);
    // signal

    if(ret_value < 0){
        return ret_value;
    }

    return readers;
}

uint16_t get_inode_writers(uint16_t inode_idx, void* shm_addr){
    uint16_t writers;

    // wait
    int8_t ret_value = fs_get_data_from_inode_uint16(inode_idx, 3, &writers, shm_addr);
    // signal

    if(ret_value < 0){
        return ret_value;
    }

    return writers;
}

uint8_t  get_inode_mode(uint16_t inode_idx, void* shm_addr){
    uint8_t mode;

    // wait
    int8_t ret_value = fs_get_data_from_inode_uint8(inode_idx, 4, &mode, shm_addr);
    // signal

    if(ret_value < 0){
        return ret_value;
    }

    return mode;
}

uint8_t  get_ref_count(uint16_t inode_idx, void* shm_addr){
    uint8_t ref_count;

    // wait
    int8_t ret_value = fs_get_data_from_inode_uint8(inode_idx, 5, &ref_count, shm_addr);
    // signal

    if(ret_value < 0){
        return ret_value;
    }

    return ref_count;
}

uint32_t used_inodes_count(void* shm_addr){
    return fs_get_used_inodes(shm_addr);
}

uint32_t used_blocks_count(void* shm_addr){
    return fs_get_used_blocks(shm_addr);
}

uint32_t used_rows_count(void* shm_addr){
    return fs_get_used_opened_files(shm_addr);
}

void print_simplefs_stats(void* shm_addr){
    printf("Address to the begin of file system: \t- %p\n\n", shm_addr);

    puts("Superblock structure:");
    struct Superblock superbl;
    fs_get_superblock_copy(&superbl, shm_addr);

    printf("Maksimum number of inodes \t- %u\n", superbl.max_number_of_inodes);
    printf("Maksimum number of opened files \t- %u\n", superbl.max_number_of_open_files);
    printf("Maksimum number of data blocks \t- %u\n", superbl.number_of_data_blocks);
    printf("Filesystem size \t- %u\n", superbl.fs_size);
    printf("Size of one block \t- %u\n", superbl.data_block_size);
    printf("Pointer to open file table \t- %u\n", superbl.open_file_table_pointer);
    printf("Pointer to open file stat \t- %u\n", superbl.open_file_stat_pointer);
    printf("Pointer to inode table \t- %u\n", superbl.inode_table_pointer);
    printf("Pointer to inode stat \t- %u\n", superbl.inode_stat_pointer);
    printf("Pointer to block links \t- %u\n", superbl.block_links_pointer);
    printf("Pointer to block stat \t- %u\n", superbl.block_stat_pointer);
    printf("Pointer to begin of data blocks \t- %u\n", superbl.data_blocks_pointer);

    puts("\n");
    printf("Open files used: \t- %u\n", fs_get_used_opened_files(shm_addr));
    printf("Inode used: \t- %u\n", fs_get_used_inodes(shm_addr));
    printf("Data blocks used: \t- %u\n", fs_get_used_blocks(shm_addr));
}

uint16_t find_free_inode(void* shm_addr){
    uint16_t ret;
    fs_get_free_inode(&ret, shm_addr);
    return ret;
}

uint16_t find_free_row(void* shm_addr){
    uint16_t ret;
    fs_get_free_open_file(&ret, shm_addr);
    return ret;
}

void use_inode(uint16_t inodeIndex, void* shm_addr){
    fs_mark_inode_as_used(inodeIndex, shm_addr);
    uint32_t tmp = fs_get_used_inodes(shm_addr);
    fs_set_used_inodes(++tmp, shm_addr);
}

void use_row(uint16_t openFileIndex, void* shm_addr){
    fs_mark_open_file_as_used(openFileIndex, shm_addr);
    uint32_t tmp = fs_get_used_opened_files(shm_addr);
    fs_set_used_opened_files(++tmp, shm_addr);
}

void free_row(uint16_t openFileIndex, void* shm_addr) {
    fs_mark_open_file_as_free(openFileIndex, shm_addr);
}

int16_t read_buffer(uint32_t block_num, uint32_t offset, char* buf, int len, void* shm_addr) {

    // this function has to return number of bytes that were read
    // change in fs_get_data() required

    // target inode read semaphore
    fs_get_data(offset, offset + len, block_num, buf, shm_addr);
    return fs_get_data_count(offset, offset + len, block_num, buf, shm_addr);
}

int16_t write_buffer(uint32_t block_num, uint32_t offset, char* buf, int len, void* shm_addr) {

    // this function has to return number of bytes that were written
    // change in fs_save_data() required

    // target inode write semaphore
    // also block_stat semaphore here or inside fs_save_data 
    // because of block allocation
    fs_save_data(offset, offset + len, block_num, buf, shm_addr);
    return fs_save_data_count(offset, offset + len, block_num, buf, shm_addr);
}


// Synchronized setters for Inode

int8_t set_inode_block_index(uint16_t inode, uint32_t block_index, void* shm_addr) {

    // can be changed only via simplefs_write
    // target inode write semaphore
    int8_t ret_value = fs_save_data_to_inode_uint32(inode, 0, block_index, shm_addr);
    return ret_value;
}

int8_t set_inode_file_size(uint16_t inode, uint16_t filesize, void* shm_addr) {

    // can be changed only via simplefs_write
    // target inode write semaphore
    int8_t ret_value = fs_save_data_to_inode_uint16(inode, 1, filesize, shm_addr);
    return ret_value;
}

int8_t set_inode_mode(uint16_t inode, uint8_t mode, void* shm_addr) {

    // no sync needed
    int8_t ret_value = fs_save_data_to_inode_uint8(inode, 4, mode, shm_addr);
    return ret_value;
}

// Synchronized setters for OpenFile

int8_t set_inode_num(uint16_t fd, uint16_t inode_num, void* shm_addr) {
    // no sync needed
    int8_t ret_value = fs_save_data_to_open_file_uint16(fd, 1, inode_num, shm_addr);
    return ret_value;
}

int8_t set_offset(uint16_t fd, uint32_t offset, void* shm_addr) {

    // no sync needed
    int8_t ret_value = fs_save_data_to_open_file_uint32(fd, 2, offset, shm_addr);
    return ret_value;
}

int16_t free_dir_entry(uint32_t dir_block_number, uint16_t inode, void *shm_addr) {
    struct DirEntry copy;
    uint32_t entry_idx = 0;
    int ret = -1;

    // wait
    while(fs_get_dir_entry_copy(dir_block_number, entry_idx, &copy, shm_addr) >= 0){
        if(copy.inode_number == inode){
            // free dir entry
            ret = fs_save_data_to_dir_entry_inode_number(dir_block_number, entry_idx, 0, shm_addr);
            break;
        }
        ++entry_idx;
    }
    // signal

    // If not found or error
    if(ret < 0)
        return -1;

    return 0;
}

int16_t free_inode(uint16_t inode, void *shm_addr) {
    // maybe sync
    int ret = fs_mark_inode_as_free(inode, shm_addr);
    if(ret < 0){
        return ret;
    }

    return 0;
}

int16_t free_data_blocks(uint32_t block_index, void *shm_addr) {
    // maybe sync
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
