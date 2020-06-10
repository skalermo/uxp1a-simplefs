
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#include "simplefs_api.h"


int main(int argc, char** argv) {
    if (argc != 2)
        exit(1);

    pid_t pid = getpid();
    int file_no = atoi(argv[1]);
    char text[50];

    int fd;
    if (file_no == 1) {
        fd = simplefs_open("/dir/file1", RDONLY);
    }
    else if (file_no == 2) {
        fd = simplefs_open("/dir/file2", RDONLY);
    } else
        exit(1);

    printf("\033[36m [Reader %d] opened file%d with fd=%d\n", pid, file_no, fd);

    srand(time(NULL) - getpid());

    sleep(2);

    while(1) {
        printf("\033[95m [Reader %d] tries to read from file%d\n", pid, file_no);

        int bytes_read = simplefs_read(fd, text, sizeof(text));
        simplefs_lseek(fd, SEEK_SET, 0);
        printf("\033[36m [Reader %d] read from file%d \033[37m'%.*s'\n", pid, file_no, bytes_read, text);

        sleep(1);
    }
}