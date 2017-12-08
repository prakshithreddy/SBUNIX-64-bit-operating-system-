#ifndef _TARFS_H
#define _TARFS_H

#include<sys/defs.h>
#include<dirent.h>
#include<sys/elf64.h>
#include<sys/task.h>

extern char _binary_tarfs_start;
extern char _binary_tarfs_end;

void initTarfs();
uint64_t validate_elf_file(Elf64_Ehdr *elf_hdr);
uint64_t map_elf_file(Elf64_Ehdr *elf_file,Elf64_Phdr *elf_p_hdr,uint64_t pml4,Task *uthread);
uint64_t loadFile(char *file,uint64_t pml4,Task *uthread);

struct posix_header_ustar {
  char name[100];
  char mode[8];
  char uid[8];
  char gid[8];
  char size[12];
  char mtime[12];
  char checksum[8];
  char typeflag[1];
  char linkname[100];
  char magic[6];
  char version[2];
  char uname[32];
  char gname[32];
  char devmajor[8];
  char devminor[8];
  char prefix[155];
  char pad[12];
};

void memcpy(void *src,void *dest,size_t n);
uint64_t getDirEntries(int fd,char *buf,int count);
int64_t closeDir(DIR *fd);
struct dirent *readDir(DIR *fd);
DIR *openDirectory(char* dirName);
uint64_t closeFile(int fd);
uint64_t readFile(int fd,char *buf,int count);
uint64_t writeFile(int fd,char *buf,int count);
int64_t openFile(char* fileName);
int64_t changeDirectory(char *buf);
char *getCWD(char *dest,int count);
void remove_dotslash(char *src,char *dest,int d);
uint64_t get_file_address(char* file);
int64_t setenv(char *name,char *value, int overwrite);
char *getenv(char *name);
int64_t env(char **com_args);

#endif
