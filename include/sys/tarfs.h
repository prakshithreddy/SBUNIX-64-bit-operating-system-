#ifndef _TARFS_H
#define _TARFS_H

#include<sys/defs.h>
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

#endif
