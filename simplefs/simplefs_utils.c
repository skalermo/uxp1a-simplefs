#include "simplefs_utils.h"
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

int16_t next_inode(uint16_t prev_inode, char* name, void* shm_addr){
    struct DirEntry copy;

    uint32_t dir_file_block = get_inode_block_index(prev_inode, shm_addr);

    uint32_t entry_idx = 0;
    while(get_dir_entry(dir_file_block, entry_idx, &copy, shm_addr) > 0){

        if(copy.inode_number != 0 && !strcmp((char*) copy.name, name)){
            return copy.inode_number;
        }
        ++entry_idx;
    }

    return -1;
}

int32_t get_inode_index(char *path, void* shm_addr){
    uint16_t current_inode = 1;
    char** sub_path = NULL;
    uint32_t sub_path_count = parse_path(path, &sub_path);

    for(int i = 0; i < sub_path_count; ++i){
        current_inode = next_inode(current_inode, sub_path[i], shm_addr);
        if(current_inode < 0) {
            return -1;
        }
    }

    free_string_array(&sub_path, sub_path_count);

    return current_inode;
}
