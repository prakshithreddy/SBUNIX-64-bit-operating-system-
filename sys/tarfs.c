#include<sys/tarfs.h>
#include<sys/elf64.h>
#include<sys/kprintf.h>
#include<sys/defs.h>
#include<sys/virtualMemory.h>
#include<sys/phyMemMapper.h>

#define MAX_SEGMENTS 100
#define ELF_PT_LOAD 1

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

void memcpy(void *src,void *dest,size_t n){
    unsigned char *s=src;
    unsigned char *d=dest;
    while(n>0){
      *d=*s;
      s++;
      d++;
      n--;
    }
}

void memset_file(void *str, int c, size_t n){
    unsigned char *s = str;
    while(n > 0){
        *s = c;
        s++;
        n--;
    }
}

uint64_t map_elf_file(Elf64_Ehdr *elf_file,Elf64_Phdr *elf_p_hdr,uint64_t pml4){
    uint64_t p_filesz=elf_p_hdr->p_filesz;
    uint64_t p_memsz=elf_p_hdr->p_memsz;
    uint64_t p_offset=elf_p_hdr->p_offset;
    uint64_t p_vaddr=elf_p_hdr->p_vaddr;
    //uint64_t p_paddr=elf_p_hdr->p_paddr; //TODO:Not sure which one to use between p_vaddr and p_paddr
    
    if(p_memsz<p_filesz){
        kprintf("MemSize of file is less than File Size.. Something wrong..\n");
        return 0;
    }
    
    //**************
    uint64_t temp=p_filesz;
    uint64_t temp2=p_vaddr;
    uint64_t temp3=p_offset;
    uint64_t phy_temp2=(uint64_t)pageAllocator();
    uint64_t vir_ker_temp2=phy_temp2+get_kernbase();
    mapPageForUser(temp2,phy_temp2,pml4);
    //mapPage(temp2,phy_temp2);
    while(temp>0){
        if(temp>0x1000){
          memcpy((void *)(elf_file+temp3),(void *)vir_ker_temp2,0x1000);
          temp2+=0x1000;
          temp-=0x1000;
          temp3+=0x1000;
          phy_temp2=(uint64_t)pageAllocator();
          vir_ker_temp2=phy_temp2+get_kernbase();
          mapPageForUser(temp2,phy_temp2,pml4);
          //mapPage(temp2,phy_temp2);
        }
        else{
          memcpy((void *)(elf_file+temp3),(void *)vir_ker_temp2,temp);
          temp2+=temp;
          vir_ker_temp2+=temp;
          temp=0;
          temp3+=temp;
        }
    }
    memset_file((void *)(vir_ker_temp2+p_filesz),0,p_memsz-p_filesz);
    //TODO: The above section of code needs to be changed to reflect vma_strcut i.e, using malloc of user process and kernel, so that this will also be accounted in vma_struct.
    //For now, allocating a page and mapping it into the correspoding user or kernel process. Mapping can also be ignore if vma_struct and demand paging are implemented.
    
    return 1;
}

uint64_t validate_elf_file(Elf64_Ehdr *elf_hdr){
	if(!elf_hdr){
      return 0;
  }
	if(elf_hdr->e_ident[0] != 0x7F){
    kprintf("Magic Number 0 incorrect\n");
    return 0;
	}
	if(elf_hdr->e_ident[1] != 'E') {
    kprintf("Magic Number 1 incorrect\n");
    return 0;
	}
	if(elf_hdr->e_ident[2] != 'L') {
    kprintf("Magic Number 2 incorrect\n");
    return 0;
	}
	if(elf_hdr->e_ident[3] != 'F') {
    kprintf("Magic Number 3 incorrect\n");
    return 0;
	}
  if(elf_hdr->e_type!= 2) {
		kprintf("File Not Executable..\n");
    return 0;
	}
	return 1;//TODO: Architecture specific checks are not done as of now.. May be important later on.
}


uint64_t loadFile(char *file,uint64_t pml4){
    uint64_t search_pointer = BINARY_TARFS_START;
    struct posix_header_ustar *tar_file_pointer = tar_file_start; 
    kprintf("\nFiles Found in tarfs:\n");
    int file_found=0;
    while(search_pointer<BINARY_TARFS_END){
        if (strcmp(tar_file_pointer->name,"")!=0){
            kprintf("    *  %s, size: %s\n",tar_file_pointer->name,tar_file_pointer->size);
        }
        if (strcmp(tar_file_pointer->name,file)==0){
            file_found=1;
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
    if (file_found==0){
        kprintf("Warning: File %s not found\n",file);
        return 0;
    }
    
    // *******Below code executes if file is present and found in tarfs**********
    Elf64_Ehdr *elf_file = (Elf64_Ehdr *)(search_pointer+512);
    char *f_size=tar_file_pointer->size;
    uint64_t elf_file_end=toInteger(f_size);
    
    uint64_t temp= (uint64_t)elf_file+(elf_file->e_phoff);
    Elf64_Phdr *elf_program_hdr= (Elf64_Phdr *)temp;
    uint64_t psegments_count = elf_file->e_phnum;
    if (!(validate_elf_file(elf_file))){
        return 0;
    }
    if(elf_file_end<((elf_file->e_phoff)+((elf_file->e_phnum)*sizeof(Elf64_Phdr)))){
        kprintf("Something Wrong with the file, Program Header is out of file range\n");
        return 0;
    }//TODO: May need to include some checks, TA's might check some cases by loading a file here.. This can possibly crash the system
    int counter=0;
    for(int i=0;i<psegments_count;i++){
        if(counter>100){
            kprintf("Already loaded 100 program sections into memory.. Not proceeding further as this might crash system..\n");
            return 0;
        }
        if((elf_program_hdr+i)->p_type == ELF_PT_LOAD){
            counter+=1;
            if (!(map_elf_file(elf_file,elf_program_hdr+i,pml4))){
                kprintf("Error while loading program into memory..\n");
                return 0;
            }
        }
    }
    
    return elf_file->e_entry;
}

