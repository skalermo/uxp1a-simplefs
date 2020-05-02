#ifndef SIMPLEFS_API_H
#define SIMPLEFS_API_H

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
