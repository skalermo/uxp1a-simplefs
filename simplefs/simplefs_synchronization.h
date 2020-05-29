/*
 * simplefs_synchronization.h
 *
 *      Author: Kordowski Mateusz
 */


#ifndef SIMPLEFS_SYNCHRONIZATION_H
#define SIMPLEFS_SYNCHRONIZATION_H

#define MAIN_FOLDER_SEM_READ_TRY_NAME "Main_folder_sem_read_try"
#define MAIN_FOLDER_SEM_READ_MUTEX_NAME "Main_folder_sem_read_mutex"
#define MAIN_FOLDER_SEM_WRITE_MUTEX_NAME "Main_folder_sem_write_mutex"
#define MAIN_FOLDER_SEM_RESOURCE_NAME "Main_folder_sem_resource"

#define INODE_SEM_NAME "Inode_sem_name_"

#define INODE_STAT_SEM_NAME "Inode_stat_sem_name"
#define BLOCK_STAT_SEM_NAME "Block_stat_sem_name"
#define OPEN_FILE_STAT_SEM_NAME "Open_file_stat_sem_name"

#define READ_TRY_MUTEX_SUFFIX "_read_try"
#define READ_MUTEX_SUFFIX "_read_mutex"
#define WRITE_MUTEX_SUFFIX "_write_mutex"
#define RESOURCE_SUFFIX "_resource"

#define SEMAPHORE_MODE S_IRUSR | S_IWUSR

#include <stdint.h>
#include <stdio.h> 
#include <semaphore.h>
#include <fcntl.h>

#include "memory/inode.h"


struct Semaphore{
    sem_t* semaphore;
};

struct ReadWriteSem{
    sem_t* readTry;
    sem_t* readMutex;
    sem_t* writeMutex;
    sem_t* resource;
    uint16_t inodeIndex;
};

/**
 * The main assumption is that the *init* functions will create and return
 * the semaphores which will be used in the other functions.
 * 
 * This strategy will minimizes the frequency of references to sem_open().
 * 
 * First you must initialize or get from the system 
 * all needed semaphores before locking or unlocking them.
 * 
 * The unlink function will work only if other processes unlink or closes their semaphores.
 * Close is faster than unlink.
 * 
 * Do not interfere with what is in structures. Functions will take care of them.
 * 
 * The created inode semaphore will be used only to this inode.
 */

int8_t fs_sem_init_main_folder(struct ReadWriteSem* sem);

int8_t fs_sem_lock_read_main_folder(struct ReadWriteSem* sem, void* addr);

int8_t fs_sem_lock_write_main_folder(struct ReadWriteSem* sem, void* addr);

int8_t fs_sem_unlock_read_main_folder(struct ReadWriteSem* sem, void* addr);

int8_t fs_sem_unlock_write_main_folder(struct ReadWriteSem* sem, void* addr);

int8_t fs_sem_close_main_folder(struct ReadWriteSem* sem);

int8_t fs_sem_unlink_main_folder();


int8_t fs_sem_init_inode(struct ReadWriteSem* sem, uint16_t inodeIndex);

int8_t fs_sem_lock_read_inode(struct ReadWriteSem* sem, void* addr);

int8_t fs_sem_lock_write_inode(struct ReadWriteSem* sem, void* addr);

int8_t fs_sem_unlock_read_inode(struct ReadWriteSem* sem, void* addr);

int8_t fs_sem_unlock_write_inode(struct ReadWriteSem* sem, void* addr);

int8_t fs_sem_close_inode(struct ReadWriteSem* sem);

int8_t fs_sem_unlink_inode(struct ReadWriteSem* sem);


int8_t fs_sem_init_inode_stat(struct Semaphore* inodeStat);

int8_t fs_sem_lock_inode_stat(struct Semaphore* inodeStat);

int8_t fs_sem_unlock_inode_stat(struct Semaphore* inodeStat);

int8_t fs_sem_close_inode_stat(struct Semaphore* inodeStat);

int8_t fs_sem_unlink_inode_stat();


int8_t fs_sem_init_block_stat(struct Semaphore* blockStat);

int8_t fs_sem_lock_block_stat(struct Semaphore* blockStat);

int8_t fs_sem_unlock_block_stat(struct Semaphore* blockStat);

int8_t fs_sem_close_block_stat(struct Semaphore* blockStat);

int8_t fs_sem_unlink_block_stat();



int8_t fs_sem_init_open_file_stat(struct Semaphore* openFileStat);

int8_t fs_sem_lock_open_file_stat(struct Semaphore* openFileStat);

int8_t fs_sem_unlock_open_file_stat(struct Semaphore* openFileStat);

int8_t fs_sem_close_open_file_stat(struct Semaphore* openFileStat);

int8_t fs_sem_unlink_open_file_stat();

/////////////////////////////
// Defined errors
/////////////////////////////

#define SIMPLEFS_ERROR_SYNCH_SEM_OPEN_READ_TRY -1
#define SIMPLEFS_ERROR_SYNCH_SEM_OPEN_READ_MUTEX -2
#define SIMPLEFS_ERROR_SYNCH_SEM_OPEN_WRITE_MUTEX -3
#define SIMPLEFS_ERROR_SYNCH_SEM_OPEN_RESOURCE -4

#define SIMPLEFS_ERROR_SYNCH_SEM_CLOSE_READ_TRY -71
#define SIMPLEFS_ERROR_SYNCH_SEM_CLOSE_READ_MUTEX -72
#define SIMPLEFS_ERROR_SYNCH_SEM_CLOSE_WRITE_MUTEX -73
#define SIMPLEFS_ERROR_SYNCH_SEM_CLOSE_RESOURCE -74

#define SIMPLEFS_ERROR_SYNCH_SEM_UNLINK_READ_TRY -81
#define SIMPLEFS_ERROR_SYNCH_SEM_UNLINK_READ_MUTEX -82
#define SIMPLEFS_ERROR_SYNCH_SEM_UNLINK_WRITE_MUTEX -83
#define SIMPLEFS_ERROR_SYNCH_SEM_UNLINK_RESOURCE -84

#define SIMPLEFS_ERROR_SYNCH_SEM_WAIT_READ_TRY -11
#define SIMPLEFS_ERROR_SYNCH_SEM_WAIT_READ_MUTEX -12
#define SIMPLEFS_ERROR_SYNCH_SEM_WAIT_WRITE_MUTEX -13
#define SIMPLEFS_ERROR_SYNCH_SEM_WAIT_RESOURCE -14

#define SIMPLEFS_ERROR_SYNCH_SEM_POST_READ_TRY -21
#define SIMPLEFS_ERROR_SYNCH_SEM_POST_READ_MUTEX -22
#define SIMPLEFS_ERROR_SYNCH_SEM_POST_WRITE_MUTEX -23
#define SIMPLEFS_ERROR_SYNCH_SEM_POST_RESOURCE -24

#define SIMPLEFS_ERROR_SYNCH_GET_DATA -31
#define SIMPLEFS_ERROR_SYNCH_SAVE_DATA -32

#define SIMPLEFS_ERROR_SYNCH_SEM_OPEN_INODE_STAT -41
#define SIMPLEFS_ERROR_SYNCH_SEM_OPEN_BLOCK_STAT -42
#define SIMPLEFS_ERROR_SYNCH_SEM_OPEN_OPEN_FILE_STAT -43

#define SIMPLEFS_ERROR_SYNCH_SEM_WAIT_INODE_STAT -51
#define SIMPLEFS_ERROR_SYNCH_SEM_WAIT_BLOCK_STAT -52
#define SIMPLEFS_ERROR_SYNCH_SEM_WAIT_OPEN_FILE_STAT -53

#define SIMPLEFS_ERROR_SYNCH_SEM_POST_INODE_STAT -61
#define SIMPLEFS_ERROR_SYNCH_SEM_POST_BLOCK_STAT -62
#define SIMPLEFS_ERROR_SYNCH_SEM_POST_OPEN_FILE_STAT -63

#define SIMPLEFS_ERROR_SYNCH_SEM_CLOSE_INODE_STAT -91
#define SIMPLEFS_ERROR_SYNCH_SEM_CLOSE_BLOCK_STAT -92
#define SIMPLEFS_ERROR_SYNCH_SEM_CLOSE_OPEN_FILE_STAT -93

#define SIMPLEFS_ERROR_SYNCH_SEM_UNLINK_INODE_STAT -111
#define SIMPLEFS_ERROR_SYNCH_SEM_UNLINK_BLOCK_STAT -112
#define SIMPLEFS_ERROR_SYNCH_SEM_UNLINK_OPEN_FILE_STAT -113


#endif