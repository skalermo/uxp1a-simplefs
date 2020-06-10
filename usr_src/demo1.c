#include <stdio.h>
#include <sys/mman.h>
#include <memory/superblock.h>
#include <semaphore.h>
#include "simplefs_api.h"
#include "memory/init.h"

int main(int argc, char const *argv[])
{
    sem_unlink(CREATE_FS_GUARD);
    shm_unlink(FS_SHM_NAME);
    char * buffer = 0;
    long length;

    FILE * f = fopen ("./usr_src/cloud.jpg", "rb");

    if (f)
    {
        fseek (f, 0, SEEK_END);
        length = ftell (f);
        rewind (f);
        buffer = malloc (length);
        if (buffer)
        {
            fread (buffer, 1, length, f);
        }
        fclose (f);
    }
    printf("%ld\n", length);
    int fd = simplefs_creat("/output.jpg", WRONLY);
    int result = 0;
    if (buffer)
    {
        result = simplefs_write(fd, buffer, length);
        if(result != length)
            printf("OH NO %d", result);
        printf("result %d\n", result);
    }

    free(buffer);
    simplefs_close(fd);

    char* result_buffer = malloc(length);
    int fd2 = simplefs_open("/output.jpg", RDONLY);
    int read_result = simplefs_read(fd2, result_buffer, result);
    printf("read result %d\n", read_result);
    FILE * dest = fopen("./usr_src/output.jpg", "wb");

    fwrite( result_buffer, 1, result, dest);
    fclose(dest);

    free(result_buffer);

    simplefs_close(fd2);
    simplefs_unlink("/output.jpg");

    unlink_fs();
    return 0;
}