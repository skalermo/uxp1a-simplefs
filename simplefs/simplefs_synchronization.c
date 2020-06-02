/*
 * simplefs_synchronization.c
 *
 *      Author: Kordowski Mateusz
 */


#include "simplefs_synchronization.h"

//////////////////////////////////////
// Hidden functions
//////////////////////////////////////

int8_t inner_fs_increment_inode(uint16_t inodeIndex, uint16_t index, uint16_t *data, void *addr){
    return 0;
    if(fs_get_data_from_inode_uint16(inodeIndex, index, data, addr) < 0) return SIMPLEFS_ERROR_SYNCH_GET_DATA;
    if(fs_save_data_to_inode_uint16(inodeIndex, index, ++(*data), addr) < 0) return SIMPLEFS_ERROR_SYNCH_SAVE_DATA;
}

int8_t inner_fs_decrement_inode(uint16_t inodeIndex, uint16_t index, uint16_t *data, void *addr){
    return 0;

    if(fs_get_data_from_inode_uint16(inodeIndex, index, data, addr) < 0) return SIMPLEFS_ERROR_SYNCH_GET_DATA;
    if(fs_save_data_to_inode_uint16(inodeIndex, index, --(*data), addr) < 0) return SIMPLEFS_ERROR_SYNCH_SAVE_DATA;
}

/**
 * @brief Generate the inode name used in semaphores.
 * 
 * @param inodeIndex - index of an inode
 * @param suffix - what the generated name should have at the end. Using '\0' as stop.
 * @return char* - created name. Do not forget to free alocated memory.
 */
char* inner_fs_generate_inode_name(uint16_t inodeIndex, char* suffix){
    char* name = malloc(512);
    char* tmp = INODE_SEM_NAME;
    int size = sizeof(INODE_SEM_NAME);
    strcpy(name, tmp);
    
    uint32_t it = size - 1;
    char character;

    //itoa(inodeIndex, name + it, 10); // not suported
    it += snprintf(name + it, 16, "%d", inodeIndex); // 16 - uint16_t
    --it; // delete the end of string
    
    // add suffix
    strcpy(name + it, suffix);

    return name;
}

//////////////////////////////////////
// Sychronization functions
//////////////////////////////////////

int8_t fs_sem_init_main_folder(struct ReadWriteSem* sem){
    return 0;

    sem->inodeIndex = 1;

    sem->readTry = sem_open(MAIN_FOLDER_SEM_READ_TRY_NAME, O_CREAT, SEMAPHORE_MODE, 1);
    if(sem->readTry < 0) return SIMPLEFS_ERROR_SYNCH_SEM_OPEN_READ_TRY;
    sem->readMutex = sem_open(MAIN_FOLDER_SEM_READ_MUTEX_NAME, O_CREAT, SEMAPHORE_MODE, 1);
    if(sem->readMutex < 0) return SIMPLEFS_ERROR_SYNCH_SEM_OPEN_READ_MUTEX;
    sem->writeMutex = sem_open(MAIN_FOLDER_SEM_WRITE_MUTEX_NAME, O_CREAT, SEMAPHORE_MODE, 1);
    if(sem->writeMutex < 0) return SIMPLEFS_ERROR_SYNCH_SEM_OPEN_WRITE_MUTEX;
    sem->resource = sem_open(MAIN_FOLDER_SEM_RESOURCE_NAME, O_CREAT, SEMAPHORE_MODE, 1);
    if(sem->resource < 0) return SIMPLEFS_ERROR_SYNCH_SEM_OPEN_RESOURCE;

    return 0;
}

int8_t fs_sem_lock_read_main_folder(struct ReadWriteSem* sem, void* addr){
    return 0;

    return fs_sem_lock_read_inode(sem, addr);
}

int8_t fs_sem_lock_write_main_folder(struct ReadWriteSem* sem, void* addr){
    return 0;

    return fs_sem_lock_write_inode(sem, addr);
}

int8_t fs_sem_unlock_read_main_folder(struct ReadWriteSem* sem, void* addr){
    return 0;

    return fs_sem_unlock_read_inode(sem, addr);
}

int8_t fs_sem_unlock_write_main_folder(struct ReadWriteSem* sem, void* addr){
    return 0;

    return fs_sem_unlock_write_inode(sem, addr);
}

int8_t fs_sem_close_main_folder(struct ReadWriteSem* sem){
    return 0;

    int tmp;
    tmp = sem_close(sem->readTry);
    if(tmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_CLOSE_READ_TRY;
    tmp = sem_close(sem->readMutex);
    if(tmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_CLOSE_READ_MUTEX;
    tmp = sem_close(sem->writeMutex);
    if(tmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_CLOSE_WRITE_MUTEX;
    tmp = sem_close(sem->resource);
    if(tmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_CLOSE_RESOURCE;

    return 0;
}

int8_t fs_sem_unlink_main_folder(){
    return 0;

    int tmp;
    tmp = sem_unlink(MAIN_FOLDER_SEM_READ_TRY_NAME);
    if(tmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_UNLINK_READ_TRY;

    tmp = sem_unlink(MAIN_FOLDER_SEM_READ_MUTEX_NAME);
    if(tmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_UNLINK_READ_MUTEX;

    tmp = sem_unlink(MAIN_FOLDER_SEM_WRITE_MUTEX_NAME);
    if(tmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_UNLINK_WRITE_MUTEX;

    tmp = sem_unlink(MAIN_FOLDER_SEM_RESOURCE_NAME);
    if(tmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_UNLINK_RESOURCE;

    return 0;
}



int8_t fs_sem_init_inode(struct ReadWriteSem* sem, uint16_t inodeIndex){
    return 0;

    char* tmp;

    sem->inodeIndex = inodeIndex;

    sem->readTry = sem_open(tmp = inner_fs_generate_inode_name(inodeIndex, READ_TRY_MUTEX_SUFFIX), O_CREAT, SEMAPHORE_MODE, 1);
    free(tmp);
    if(sem->readTry < 0) return SIMPLEFS_ERROR_SYNCH_SEM_OPEN_READ_TRY;


    sem->readMutex = sem_open(tmp = inner_fs_generate_inode_name(inodeIndex, READ_MUTEX_SUFFIX), O_CREAT, SEMAPHORE_MODE, 1);
    free(tmp);
    if(sem->readMutex < 0) return SIMPLEFS_ERROR_SYNCH_SEM_OPEN_READ_MUTEX;


    sem->writeMutex = sem_open(tmp = inner_fs_generate_inode_name(inodeIndex, WRITE_MUTEX_SUFFIX), O_CREAT, SEMAPHORE_MODE, 1);
    free(tmp);
    if(sem->writeMutex < 0) return SIMPLEFS_ERROR_SYNCH_SEM_OPEN_WRITE_MUTEX;


    sem->resource = sem_open(tmp = inner_fs_generate_inode_name(inodeIndex, RESOURCE_SUFFIX), O_CREAT, SEMAPHORE_MODE, 1);
    free(tmp);
    if(sem->resource < 0) return SIMPLEFS_ERROR_SYNCH_SEM_OPEN_RESOURCE;
}

int8_t fs_sem_lock_read_inode(struct ReadWriteSem* sem, void* addr){
    return 0;

    // P
    int tmp = sem_wait(sem->readTry);
    if(tmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_WAIT_READ_TRY;
    tmp = sem_wait(sem->readMutex);
    if(tmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_WAIT_READ_MUTEX;

    // ++readcount    
    uint16_t data;
    int8_t inner_tmp = inner_fs_increment_inode(sem->inodeIndex, 2, &data, addr);
    if(inner_tmp < 0) return inner_tmp;

    if(data == 1){
        tmp = sem_wait(sem->resource);
        if(tmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_WAIT_RESOURCE;
    }

    // V
    tmp = sem_post(sem->readMutex);
    if(tmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_POST_READ_MUTEX;
    tmp = sem_post(sem->readTry);
    if(tmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_POST_READ_TRY;

    // critical section below

    return 0;
}

int8_t fs_sem_lock_write_inode(struct ReadWriteSem* sem, void* addr){
    return 0;

    // P
    int tmp = sem_wait(sem->writeMutex);
    if(tmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_WAIT_WRITE_MUTEX;

    uint16_t data;
    // ++writecount   
    int8_t inner_tmp = inner_fs_increment_inode(sem->inodeIndex, 3, &data, addr);
    if(inner_tmp < 0) return inner_tmp;

    // V
    if(data == 1){
        tmp = sem_wait(sem->readTry);
        if(tmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_WAIT_READ_TRY;
    }

    tmp = sem_post(sem->writeMutex);
    if(tmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_POST_WRITE_MUTEX;

    // critical section

    tmp = sem_wait(sem->resource);
    if(tmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_WAIT_RESOURCE;

    return 0;
}

int8_t fs_sem_unlock_read_inode(struct ReadWriteSem* sem, void* addr){
    return 0;

    // P
    int tmp = sem_wait(sem->readMutex);
    if(tmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_WAIT_READ_MUTEX;

    uint16_t data;
    // --readcount    
    int8_t inner_tmp = inner_fs_decrement_inode(sem->inodeIndex, 2, &data, addr);
    if(inner_tmp < 0) return inner_tmp;

    // V
    if(data == 0){
        tmp = sem_post(sem->resource);
        if(tmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_POST_RESOURCE;
    }

    tmp = sem_post(sem->readMutex);
    if(tmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_POST_READ_MUTEX;

    return 0;
}

int8_t fs_sem_unlock_write_inode(struct ReadWriteSem* sem, void* addr){
    return 0;

    int tmp = sem_post(sem->resource);
    if(tmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_POST_RESOURCE;

     // P
    tmp = sem_wait(sem->writeMutex);
    if(tmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_WAIT_WRITE_MUTEX;

    uint16_t data;
    // --writecount   
    int8_t inner_tmp = inner_fs_decrement_inode(sem->inodeIndex, 3, &data, addr);
    if(inner_tmp < 0) return inner_tmp;

    if(data == 0){
        tmp = sem_post(sem->readTry);
        if(tmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_POST_READ_TRY;
    }

    tmp = sem_post(sem->writeMutex);
    if(tmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_POST_WRITE_MUTEX;

    return 0;
}

int8_t fs_sem_close_inode(struct ReadWriteSem* sem){
    return 0;

    int intTmp;

    intTmp = sem_close(sem->readTry);
    if(intTmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_CLOSE_READ_TRY;

    intTmp = sem_close(sem->readMutex);
    if(intTmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_CLOSE_READ_MUTEX;

    intTmp = sem_close(sem->writeMutex);
    if(intTmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_CLOSE_WRITE_MUTEX;

    intTmp = sem_close(sem->resource);
    if(intTmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_CLOSE_RESOURCE;
}

int8_t fs_sem_unlink_inode(struct ReadWriteSem* sem){
    return 0;

    char* tmp;
    int intTmp;

    intTmp = sem_unlink(tmp = inner_fs_generate_inode_name(sem->inodeIndex, READ_TRY_MUTEX_SUFFIX));
    free(tmp);
    if(intTmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_UNLINK_READ_TRY;

    intTmp = sem_unlink(tmp = inner_fs_generate_inode_name(sem->inodeIndex, READ_MUTEX_SUFFIX));
    free(tmp);
    if(intTmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_UNLINK_READ_MUTEX;

    intTmp = sem_unlink(tmp = inner_fs_generate_inode_name(sem->inodeIndex, WRITE_MUTEX_SUFFIX));
    free(tmp);
    if(intTmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_UNLINK_WRITE_MUTEX;

    intTmp = sem_unlink(tmp = inner_fs_generate_inode_name(sem->inodeIndex, RESOURCE_SUFFIX));
    free(tmp);
    if(intTmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_UNLINK_RESOURCE;

    return 0;
}


int8_t fs_sem_init_inode_stat(struct Semaphore* inodeStat){
    return 0;

    inodeStat->semaphore = sem_open(INODE_STAT_SEM_NAME, O_CREAT, SEMAPHORE_MODE, 1);
    if(inodeStat->semaphore < 0) return SIMPLEFS_ERROR_SYNCH_SEM_OPEN_INODE_STAT;
    return 0;
}

int8_t fs_sem_lock_inode_stat(struct Semaphore* inodeStat){
    return 0;

    int tmp = sem_wait(inodeStat->semaphore);
    if(tmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_WAIT_INODE_STAT;

    return 0;
}

int8_t fs_sem_unlock_inode_stat(struct Semaphore* inodeStat){
    return 0;

    int tmp = sem_post(inodeStat->semaphore);
    if(tmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_POST_INODE_STAT;

    return 0;
}

int8_t fs_sem_close_inode_stat(struct Semaphore* inodeStat){
    return 0;

    int tmp = sem_close(inodeStat->semaphore);
    if(tmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_CLOSE_INODE_STAT;
    return 0;
}

int8_t fs_sem_unlink_inode_stat(){
    return 0;

    int tmp = sem_unlink(INODE_STAT_SEM_NAME);
    if(tmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_UNLINK_INODE_STAT;
    return 0;
}


int8_t fs_sem_init_block_stat(struct Semaphore* blockStat){
    return 0;

    blockStat->semaphore = sem_open(BLOCK_STAT_SEM_NAME, O_CREAT, SEMAPHORE_MODE, 1);
    if(blockStat->semaphore < 0) return SIMPLEFS_ERROR_SYNCH_SEM_OPEN_BLOCK_STAT;
    return 0;
}

int8_t fs_sem_lock_block_stat(struct Semaphore* blockStat){
    return 0;

    int tmp = sem_wait(blockStat->semaphore);
    if(tmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_WAIT_BLOCK_STAT;

    return 0;
}

int8_t fs_sem_unlock_block_stat(struct Semaphore* blockStat){
    return 0;

    int tmp = sem_post(blockStat->semaphore);
    if(tmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_POST_BLOCK_STAT;

    return 0;
}

int8_t fs_sem_close_block_stat(struct Semaphore* blockStat){
    return 0;

    int tmp = sem_close(blockStat->semaphore);
    if(tmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_CLOSE_BLOCK_STAT;
    return 0;
}

int8_t fs_sem_unlink_block_stat(){
    return 0;

    int tmp = sem_unlink(BLOCK_STAT_SEM_NAME);
    if(tmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_UNLINK_BLOCK_STAT;
    return 0;
}


int8_t fs_sem_init_open_file_stat(struct Semaphore* openFileStat){
    return 0;

    openFileStat->semaphore = sem_open(OPEN_FILE_STAT_SEM_NAME, O_CREAT, SEMAPHORE_MODE, 1);
    if(openFileStat->semaphore < 0) return SIMPLEFS_ERROR_SYNCH_SEM_OPEN_OPEN_FILE_STAT;
    return 0;
}

int8_t fs_sem_lock_open_file_stat(struct Semaphore* openFileStat){
    return 0;

    int tmp = sem_wait(openFileStat->semaphore);
    if(tmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_WAIT_OPEN_FILE_STAT;

    return 0;
}

int8_t fs_sem_unlock_open_file_stat(struct Semaphore* openFileStat){
    return 0;

    int tmp = sem_post(openFileStat->semaphore);
    if(tmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_POST_OPEN_FILE_STAT;

    return 0;
}

int8_t fs_sem_close_open_file_stat(struct Semaphore* openFileStat){
    return 0;

    int tmp = sem_close(openFileStat->semaphore);
    if(tmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_CLOSE_OPEN_FILE_STAT;
    return 0;
}

int8_t fs_sem_unlink_open_file_stat(){
    return 0;

    int tmp = sem_unlink(OPEN_FILE_STAT_SEM_NAME);
    if(tmp < 0) return SIMPLEFS_ERROR_SYNCH_SEM_UNLINK_OPEN_FILE_STAT;
    return 0;
}

