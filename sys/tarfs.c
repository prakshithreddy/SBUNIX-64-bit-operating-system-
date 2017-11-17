#include<sys/tarfs.h>
#include<sys/elf64.h>
#include<sys/kprintf.h>

static uint64_t BINARY_TARFS_START, BINARY_TARFS_END;
static struct posix_header_ustar *tar_file_start;
 
void initTarfs(){
    BINARY_TARFS_START = (uint64_t)&_binary_tarfs_start;
    BINARY_TARFS_END = (uint64_t)&_binary_tarfs_end;
    tar_file_start = (struct posix_header_ustar *)(BINARY_TARFS_START);
}

int strcmp(char *str1,char *str2){
  int i=0;
  while(*(str1+i) != '\0' || *(str2+i) != '\0'){
    if(*(str1+i) == *(str2+i))
      i++;
    else{
      return *(str1+i) - *(str2+i);
    }
  }
  return 0;
}

int toInteger(char *s){
  uint64_t i=0;
  int num=0;
  while(*(s+i)!= '\0'){
    num=num*8+(*(s+i)-'0');
    i+=1;
  }
  return num;
}

struct Elf64_Ehdr *findFile(char *file){
    uint64_t search_pointer = BINARY_TARFS_START;
    struct posix_header_ustar *tar_file_pointer = tar_file_start; 
    kprintf("\nFiles Found in tarfs:\n");
    while(search_pointer<BINARY_TARFS_END){
        if (strcmp(tar_file_pointer->name,"")!=0){
            kprintf("    *  %s, size: %s\n",tar_file_pointer->name,tar_file_pointer->size);
        }
        if (strcmp(tar_file_pointer->name,file)==0){
            break;
        }
        char *f_size=tar_file_pointer->size;
        int size = toInteger(f_size);
        int residue = size%512;
        if(residue>0){
          size=size+(512-residue);
        }
        search_pointer=search_pointer+size+512;
        tar_file_pointer = (struct posix_header_ustar *)search_pointer;
    }
    return (struct Elf64_Ehdr *)(search_pointer+512);
}

