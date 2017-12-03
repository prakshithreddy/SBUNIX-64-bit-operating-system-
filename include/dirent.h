#ifndef _DIRENT_H
#define _DIRENT_H

#include <sys/defs.h>

#define NAME_MAX 255

struct dirent {
    unsigned long  d_ino;
    unsigned long  d_off;
    unsigned short d_reclen;
    char           d_name[NAME_MAX+1];
}Dirent;

typedef struct fileDescriptor{
    uint64_t fd;
    uint64_t start;
    uint64_t offset;
    char d_prev_dir[NAME_MAX+1];
    char name[NAME_MAX+1];
}FileDescriptor;

typedef struct directory{
    uint64_t d_start;
    uint64_t d_offset;
    char dirName[NAME_MAX+1];
    char* d_prev_dir;
}Directory;

typedef struct DIR DIR;

void list_all_dir(const char* path);

DIR *opendir(const char *name);
struct dirent *readdir(DIR *dirp);
int closedir(DIR *dirp);

int chdir(const char *path);


int getdents(unsigned int fd, char *dirp,unsigned int count);

#endif
