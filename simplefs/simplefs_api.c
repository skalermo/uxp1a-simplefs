#ifndef SIMPLEFS_API_C
#define SIMPLEFS_API_C

#include "simplefs_api.h"
#include "simplefs_utils.h"
#include "simplefs_synchronization.h"
#include "memory/init.h"
#include <libgen.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>


void* shm_addr = NULL;

int simplefs_open(char *name, int mode) {
    // Init system
    shm_addr = get_ptr_to_fs();

    struct ReadWriteSem mainFolder;
    struct Semaphore openFile;
    struct ReadWriteSem semInode;

    fs_sem_init_main_folder(&mainFolder);

    // Get Inode idx for file
    fs_sem_lock_read_main_folder(&mainFolder, shm_addr);
    int inode_idx = get_inode_index(name, IS_FILE, shm_addr);
    fs_sem_unlock_read_main_folder(&mainFolder, shm_addr);
    fs_sem_close_main_folder(&mainFolder);

    if(inode_idx < 0)
        return ENOTDIR;
        
    // Create Open File entry
    struct OpenFile new_open_file;
    new_open_file.mode = mode;
    new_open_file.inode_num = inode_idx;
    new_open_file.offset = 0;
    new_open_file.parent_pid = getpid();

    fs_sem_init_open_file_stat(&openFile);
    fs_sem_init_inode(&semInode, inode_idx);

    fs_sem_lock_open_file_stat(&openFile);
    fs_sem_lock_write_inode(&semInode, shm_addr);

    

    // Save Open file entry in FS
    int32_t fd = save_new_OpenFile_entry(&new_open_file, shm_addr);
    fs_sem_unlock_open_file_stat(&openFile);
    fs_sem_close_open_file_stat(&openFile);

    // Increment ref_count        
    inc_ref_count(inode_idx, shm_addr);
    
    fs_sem_unlock_write_inode(&semInode, shm_addr);
    fs_sem_close_inode(&semInode);

    return fd;
}

/**
 * Trzeba dodać do mode, czy to co tworzymy jest plikiem, czy też katalogiem w inode
 */
int simplefs_creat(char *name, int mode) {
    // Init system
    shm_addr = get_ptr_to_fs();

    struct ReadWriteSem semMainDir;
    struct ReadWriteSem semInode;
    struct Semaphore semBlock;
    struct Semaphore semInodeStat;
    struct Semaphore semOpenFile;

    fs_sem_init_main_folder(&semMainDir);
    fs_sem_lock_write_main_folder(&semMainDir, shm_addr);

    // Check if exists
    int32_t inode_idx = -1;//get_inode_index(name, shm_addr);
    if(inode_idx < 0) {
        char *name_copy = strdup(name);

        // Get Filename and dir path
        char* filename = basename(name);
        char* dir_path = dirname(name_copy);
        char* filenamePrev = basename(dir_path);

        // Get Inode idx for dir
        int dir_inode = get_inode_index(dir_path, IS_DIR, shm_addr);


            if (dir_inode < 0){
                fs_sem_unlock_write_main_folder(&semMainDir, shm_addr);
                fs_sem_close_main_folder(&semMainDir);
                return ENOTDIR;
            }

        // Create inode
        struct Inode new_inode = {0};
        new_inode.mode = IS_FILE;

        fs_sem_init_inode_stat(&semInodeStat);
        fs_sem_lock_inode_stat(&semInodeStat);
        fs_sem_init_block_stat(&semBlock);
        fs_sem_lock_block_stat(&semBlock);

        new_inode.block_index = allocate_new_chain(shm_addr);

        if(new_inode.block_index == UINT32_MAX){
            fs_sem_unlock_write_main_folder(&semMainDir, shm_addr);
            fs_sem_close_main_folder(&semMainDir);
            fs_sem_unlock_block_stat(&semBlock);
            fs_sem_close_block_stat(&semBlock);
            fs_sem_unlock_inode_stat(&semInodeStat);
            fs_sem_close_inode_stat(&semInodeStat);
            return EIO;
        }
            

        // Save inode in FS        
        inode_idx = save_new_inode(&new_inode, shm_addr);

        fs_sem_unlock_inode_stat(&semInodeStat);
        fs_sem_close_inode_stat(&semInodeStat);

        if (inode_idx < 0){
            fs_sem_unlock_write_main_folder(&semMainDir, shm_addr);
            fs_sem_close_main_folder(&semMainDir);
            fs_sem_unlock_block_stat(&semBlock);
            fs_sem_close_block_stat(&semBlock);
            return ENOSPC;
        }
            
        // it is important that we create dir entry after allocation of inode
        // because otherwise some process could starts using this inode before we create it.

        // Create Dir Entry
        struct DirEntry new_dir_entry;
        new_dir_entry.inode_number = inode_idx;
        strcpy(new_dir_entry.name, filename);
        new_dir_entry.name_len = strlen(filename);

        // Get directory block index
        uint32_t dir_block = get_inode_block_index(dir_inode, shm_addr);
        if (dir_block == INT32_MAX){
            fs_sem_unlock_write_main_folder(&semMainDir, shm_addr);
            fs_sem_close_main_folder(&semMainDir);
            fs_sem_unlock_block_stat(&semBlock);
            fs_sem_close_block_stat(&semBlock);
            return ENOENT;
        }
            
        // Save dir entry in FS
        int32_t dir_entry_idx = save_new_dir_entry(dir_block, &new_dir_entry, shm_addr);

        fs_sem_unlock_block_stat(&semBlock);
        fs_sem_close_block_stat(&semBlock);

        if (dir_entry_idx < 0){
            fs_sem_unlock_write_main_folder(&semMainDir, shm_addr);
            fs_sem_close_main_folder(&semMainDir);
            return ENOSPC;
        }
            

        free(name_copy);
    }

    // Create Open File entry
    struct OpenFile new_open_file;
    new_open_file.mode = mode;
    new_open_file.inode_num = inode_idx;
    new_open_file.offset = 0;
    new_open_file.parent_pid = getpid();

    fs_sem_init_inode(&semInode, inode_idx);
    fs_sem_lock_write_inode(&semInode, shm_addr);

    fs_sem_init_open_file_stat(&semOpenFile);
    fs_sem_lock_open_file_stat(&semOpenFile);

    // I think it is safer that way
    fs_sem_unlock_write_main_folder(&semMainDir, shm_addr);
    fs_sem_close_main_folder(&semMainDir);
    
    // Save Open file entry in FS
    int32_t fd = save_new_OpenFile_entry(&new_open_file, shm_addr);

    fs_sem_unlock_open_file_stat(&semOpenFile);
    fs_sem_close_open_file_stat(&semOpenFile);

    // Increment ref_count
    inc_ref_count(inode_idx, shm_addr);

    fs_sem_unlock_write_inode(&semInode, shm_addr);
    fs_sem_close_inode(&semInode);

    return fd;
}

/**
 * Zmienia w inode mode, czeka, jeżeli mode jest niekompatybilny z tym, co chce zrobić
 * lub nie zmienia nic, bo już dany mode jest taki jaki chce (tylko mode do read)
 * 
 * Zmienia offset w open file
 * 
 * Po wyjściu zmienia mode w inode na 0, jeżeli nikt inny nie czyta. Jeżeli czytają, to nie zmieniamy.
 * Jeżeli write, to i tak inne read czekają, więc można mode wyzerować.
 */
int simplefs_read(int fd, char *buf, int len) {
    // Init system
    shm_addr = get_ptr_to_fs();

    struct OpenFile openFile = get_open_file(fd, shm_addr);
    if(openFile.mode != FS_READ){
        return EBADF;
    }

    // Set inode mode to READ
    // not neccesary
    //set_inode_mode(openFile.inode_num, openFile.mode, shm_addr);

    // Get data block
    struct ReadWriteSem semInode;
    fs_sem_init_inode(&semInode, openFile.inode_num);
    fs_sem_lock_read_inode(&semInode, shm_addr);
    uint32_t block_idx = get_inode_block_index(openFile.inode_num, shm_addr);

    // Read Buffer
    read_buffer(block_idx, openFile.offset, buf, len, shm_addr);
    fs_sem_unlock_read_inode(&semInode, shm_addr);
    fs_sem_close_inode(&semInode);

    // Update offset
    openFile.offset += len;
    set_offset(fd, openFile.offset, shm_addr);

    // Set inode mode to 0
    // not neccesary
    //set_inode_mode(openFile.inode_num, 0, shm_addr);

    return 0;
}


/**
 * To samo co w read tylko z sprawdzaniem, czy alokujemy nowe bloki, czy też nie.
 * Ważne przy synchronizacji.
 */
int simplefs_write(int fd, char *buf, int len) {
    // Init system
    shm_addr = get_ptr_to_fs();

    struct OpenFile openFile = get_open_file(fd, shm_addr);
    if(openFile.mode != FS_WRITE){
        return EBADF;
    }

    // Set inode mode to WRITE
    // not needed
    //set_inode_mode(openFile.inode_num, openFile.mode, shm_addr);

    struct ReadWriteSem semInode;
    fs_sem_init_inode(&semInode, openFile.inode_num);
    fs_sem_lock_write_inode(&semInode, shm_addr);

    // Get data block
    uint32_t block_idx = get_inode_block_index(openFile.inode_num, shm_addr);

    /*
    check if it is needed to allocate new block
    {
        struct Semaphor semBlock;
        fs_sem_init_block_stat(&semBlock);
        fs_sem_lock_block_stat(&semBlock);

        write_buffer(block_idx, openFile.offset, buf, len, shm_addr);

        fs_sem_unlock_block_stat(&semBlock);
        fs_sem_close_block_stat(&semBlock);

    }
    else{
        write_buffer(block_idx, openFile.offset, buf, len, shm_addr);
    }
    */
    // Write Buffer
    write_buffer(block_idx, openFile.offset, buf, len, shm_addr);

    fs_sem_unlock_write_inode(&semInode, shm_addr);
    fs_sem_close_inode(&semInode);

    // Update offset
    openFile.offset += len;
    set_offset(fd, openFile.offset, shm_addr);

    // Set inode mode to 0
    // not needed
    //set_inode_mode(openFile.inode_num, 0, shm_addr);

    return 0;
}


/**
 * Zmiana offset w open file.
 * 
 * Sprawdzenie czy offset nie przekracza file size
 */
int simplefs_lseek(int fd, int whence, int offset) {
    // this does not need synchronization
    return ENOTIMPLEMENTED;
}


/**
 * Oznaczenie w bitmapie jako pusty w inode stat.
 * 
 * Usunięcie w dir file struktury odwołującej się do danego inoda
 * 
 * Sprawdzenie ref count.
 */
int simplefs_unlink(char *name) {
    // Init system
    shm_addr = get_ptr_to_fs();

    struct ReadWriteSem semMainFolder;
    struct ReadWriteSem semInode;
    struct Semaphore semInodeStat;
    struct Semaphore semBlock;

    fs_sem_init_main_folder(&semMainFolder);
    fs_sem_lock_write_main_folder(&semMainFolder, shm_addr);

    // Get file inode
    int file_inode = get_inode_index(name, IS_FILE, shm_addr);
    if(file_inode < 0){
        fs_sem_unlock_write_main_folder(&semMainFolder, shm_addr);
        fs_sem_close_main_folder(&semMainFolder);
        return file_inode;
    }

    fs_sem_init_inode(&semInode, file_inode);
    fs_sem_lock_read_inode(&semInode, shm_addr);

    // Check ref_count
    int ref_count = get_ref_count(file_inode, shm_addr);
    if(ref_count > 0){
        fs_sem_unlock_write_main_folder(&semMainFolder, shm_addr);
        fs_sem_close_main_folder(&semMainFolder);
        fs_sem_unlock_read_inode(&semInode, shm_addr);
        fs_sem_close_inode(&semInode);
        return EBUSY;
    }
        

    // Get dir inode
    char* name_copy = strdup(name);
    char* dir_path = dirname(name_copy);
    int dir_inode = get_inode_index(dir_path, IS_DIR, shm_addr);

    if(dir_inode < 0){
        fs_sem_unlock_write_main_folder(&semMainFolder, shm_addr);
        fs_sem_close_main_folder(&semMainFolder);
        fs_sem_unlock_read_inode(&semInode, shm_addr);
        fs_sem_close_inode(&semInode);
        return dir_inode;
    }

    // Get directory block index
    uint32_t dir_block = get_inode_block_index(dir_inode, shm_addr);
    if(dir_block == INT32_MAX){
        fs_sem_unlock_write_main_folder(&semMainFolder, shm_addr);
        fs_sem_close_main_folder(&semMainFolder);
        fs_sem_unlock_read_inode(&semInode, shm_addr);
        fs_sem_close_inode(&semInode);
        return ENOENT;
    }

        

    // Remove dir entry from dir file
    int ret = free_dir_entry(dir_block, file_inode, shm_addr);
    if(ret < 0){
        fs_sem_unlock_write_main_folder(&semMainFolder, shm_addr);
        fs_sem_close_main_folder(&semMainFolder);
        fs_sem_unlock_read_inode(&semInode, shm_addr);
        fs_sem_close_inode(&semInode);
        return ret;
    }

    // Set bit in bitmap
    fs_sem_init_inode_stat(&semInodeStat);
    fs_sem_lock_inode_stat(&semInodeStat);
    ret = free_inode(file_inode, shm_addr);
    fs_sem_unlock_inode_stat(&semInodeStat);
    fs_sem_close_inode_stat(&semInodeStat);

    fs_sem_unlock_write_main_folder(&semMainFolder, shm_addr);
    fs_sem_close_main_folder(&semMainFolder);

    if(ret < 0){
        return ret;
    }

    // Free blocks
    fs_sem_init_block_stat(&semBlock);
    fs_sem_lock_block_stat(&semBlock);
    uint32_t file_block = get_inode_block_index(file_inode, shm_addr);
    ret = free_data_blocks(file_block, shm_addr);
    fs_sem_unlock_block_stat(&semBlock);
    fs_sem_close_block_stat(&semBlock);

    if(ret < 0){
        return ret;
    }

    fs_sem_unlock_read_inode(&semInode, shm_addr);
    fs_sem_unlink_inode(&semInode);

    if(ret < 0){
        return ret;
    }

    return 0;
}


/**
 * Trzeba jeszcze usunąć zmiany, jeżeli operacja się nie udała
 */
int simplefs_mkdir(char *name) {
    int name_size = strlen(name);
    if(name_size < 2){
        return ENOENT;
    }

    if(name_size >= PATH_MAX){
        return ENAMETOOLONG;
    }
        

    if(shm_addr == NULL){
        shm_addr = get_ptr_to_fs();
    }
        

    char* name_copy = strdup(name);

    // Get Filename and dir path
    char* filename = basename(name);
    if(strlen(filename) >= NAME_MAX){
        return ENAMETOOLONG;
    }

    char* dir_path = dirname(name_copy);
    char* filenamePrev = basename(dir_path);

    struct ReadWriteSem semMainFolder;
    struct Semaphore semInodeStat;
    struct Semaphore semBlock;

    fs_sem_init_main_folder(&semMainFolder);
    fs_sem_lock_write_main_folder(&semMainFolder, shm_addr);

    // Get Inode idx for dir
    int dir_inode = get_inode_index(dir_path, IS_DIR, shm_addr);

    // if path is wrong
    // or if the last inode should not be dir / file
    if(dir_inode < 0){
        fs_sem_unlock_write_main_folder(&semMainFolder, shm_addr);
        fs_sem_close_main_folder(&semMainFolder);
        return ENOTDIR;
    }
    
    int32_t does_already_exist = next_inode(dir_inode, filename, IS_DIR, shm_addr);
    if(does_already_exist != -1){
        fs_sem_unlock_write_main_folder(&semMainFolder, shm_addr);
        fs_sem_close_main_folder(&semMainFolder);
        return EEXIST;
    }


    // Create DirFile and inode
    struct Inode new_inode = {0};
    new_inode.mode = IS_DIR;
    struct FS_create_dir_data structHelp;

    fs_sem_init_inode_stat(&semInodeStat);
    fs_sem_lock_inode_stat(&semInodeStat);
    fs_sem_init_block_stat(&semBlock);
    fs_sem_lock_block_stat(&semBlock);

    uint16_t inode_idx = save_new_inode(&new_inode, shm_addr);


    if(inode_idx == UINT16_MAX){
        fs_sem_unlock_write_main_folder(&semMainFolder, shm_addr);
        fs_sem_close_main_folder(&semMainFolder);
        fs_sem_unlock_inode_stat(&semInodeStat);
        fs_sem_close_inode_stat(&semInodeStat);
        fs_sem_unlock_block_stat(&semBlock);
        fs_sem_close_block_stat(&semBlock);
        return ENOSPC;
    }
   

    structHelp.prevoiusDirInode = dir_inode;
    structHelp.prevoiusDirInodeName = filenamePrev;
    structHelp.thisDirInode = inode_idx;
    structHelp.thisDirName = filename;

    // it only creates dir file in one data block. It does not modify inodes
    int8_t ret = fs_create_dir_file(&new_inode.block_index, &structHelp, shm_addr);

    if(ret < 0){
        fs_sem_unlock_write_main_folder(&semMainFolder, shm_addr);
        fs_sem_close_main_folder(&semMainFolder);
        fs_sem_unlock_inode_stat(&semInodeStat);
        fs_sem_close_inode_stat(&semInodeStat);
        fs_sem_unlock_block_stat(&semBlock);
        fs_sem_close_block_stat(&semBlock);
        return ENOSPC;
    }
        
        
    fs_save_data_to_inode_uint32(inode_idx, 0, new_inode.block_index, shm_addr);

    // Create Dir Entry
    struct DirEntry new_dir_entry;
    new_dir_entry.inode_number = inode_idx;
    strcpy(new_dir_entry.name, filename);
    new_dir_entry.name_len = strlen(filename);

    // Get previous directory block index
    uint32_t dir_block = get_inode_block_index(dir_inode, shm_addr);
    if(dir_block == INT32_MAX){
        fs_sem_unlock_write_main_folder(&semMainFolder, shm_addr);
        fs_sem_close_main_folder(&semMainFolder);
        fs_sem_unlock_inode_stat(&semInodeStat);
        fs_sem_close_inode_stat(&semInodeStat);
        fs_sem_unlock_block_stat(&semBlock);
        fs_sem_close_block_stat(&semBlock);
        return ENOENT;
    }
        
    // Modify previous Dir File and save dir entry in FS
    int32_t dir_entry_idx = save_new_dir_entry(dir_block, &new_dir_entry, shm_addr);
    fs_sem_unlock_write_main_folder(&semMainFolder, shm_addr);
    fs_sem_close_main_folder(&semMainFolder);
    fs_sem_unlock_inode_stat(&semInodeStat);
    fs_sem_close_inode_stat(&semInodeStat);
    fs_sem_unlock_block_stat(&semBlock);
    fs_sem_close_block_stat(&semBlock);

    if(dir_entry_idx < 0)
        return ENOSPC;

    free(name_copy);
    return 0;
}


/**
 * Podobne do simplefs_unlink, tylko zmieniamy dir file zamiast inodow
 * 
 */
int simplefs_rmdir(char *name) {
    return ENOTIMPLEMENTED;
}


/**
 * Usunięcie w open file.
 */
int simplefs_close(int fd) {
    if(shm_addr == NULL)
        shm_addr = get_ptr_to_fs();

    struct ReadWriteSem semInode;
    struct Semaphore semOpenFile;


    struct OpenFile file;
    fs_get_open_file_copy(fd, &file, shm_addr);

    if(file.parent_pid != getpid()){
        return EBADF;
    }

    fs_sem_init_inode(&semInode, file.inode_num);
    fs_sem_lock_write_inode(&semInode, shm_addr);
    
    uint8_t refCountInode;
    fs_get_data_from_inode_uint8(file.inode_num, 5, &refCountInode, shm_addr);
    fs_save_data_to_inode_uint8(file.inode_num, 5, --refCountInode, shm_addr);


    fs_sem_init_open_file_stat(&semOpenFile);
    fs_sem_lock_open_file_stat(&semOpenFile);
    fs_sem_unlock_write_inode(&semInode, shm_addr);
    fs_sem_unlink_inode(&semInode);

    fs_mark_open_file_as_free(fd, shm_addr);

    fs_sem_unlock_open_file_stat(&semOpenFile);
    fs_sem_close_open_file_stat(&semOpenFile);

    return 0;
}



#endif