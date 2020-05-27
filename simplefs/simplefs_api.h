#ifndef SIMPLEFS_API_H
#define SIMPLEFS_API_H

// Defines of API errors

#define EINVAL -2
#define ENOTDIR -3
#define ENAMETOOLONG -4
#define ENFILE -5
#define ENOENT -6
#define EBADF -7
#define EIO -8
#define EOVERFLOW -9
#define EFBIG -10
#define ENOSPC -11
#define EBUSY -12
#define EEXIST -13
#define EMLINK -14
#define ENOTEMPTY -15
#define EMFILE -16

#define ENOTIMPLEMENTED -100

#define FS_READ 1
#define FS_WRITE 2

// Prototypes of API functions

int simplefs_open(char *name, int mode);

int simplefs_creat(char *name, int mode);

int simplefs_read(int fd, char *buf, int len);

int simplefs_write(int fd, char *buf, int len);

int simplefs_lseek(int fd, int whence, int offset);

int simplefs_unlink(char *name);

int simplefs_mkdir(char *name);

int simplefs_rmdir(char *name);

int simplefs_close(int fd);

#endif // SIMPLEFS_API_H
