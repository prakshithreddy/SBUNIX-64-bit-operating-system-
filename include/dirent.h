#ifndef _DIRENT_H
#define _DIRENT_H

#include <sys/defs.h>

#define NAME_MAX 255

struct dirent {
    unsigned long  d_ino;
    unsigned long  d_off;
    unsigned short d_reclen;
    char           d_name[NAME_MAX+1];
};

typedef struct file{
    uint64_t f_start;
    uint64_t f_offset;
    char fileName[256];
}File;

typedef struct directory{
    uint64_t d_start;
    char dirName[256];
}Directory;

typedef struct DIR DIR;

void list_all_dir(const char* path);

DIR *opendir(const char *name);
struct dirent *readdir(DIR *dirp);
int closedir(DIR *dirp);

int chdir(const char *path);


int getdents(unsigned int fd, char *dirp,unsigned int count);

#endif
