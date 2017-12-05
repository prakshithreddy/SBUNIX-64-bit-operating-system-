#include<sys/tarfs.h>
#include<sys/elf64.h>
#include<sys/kprintf.h>
#include<sys/defs.h>
#include<sys/virtualMemory.h>
#include<sys/phyMemMapper.h>
#include<sys/task.h>
#include<dirent.h>
#include<sys/idt.h>

#define MAX_SEGMENTS 100
#define ELF_PT_LOAD 1

static uint64_t BINARY_TARFS_START, BINARY_TARFS_END;
static struct posix_header_ustar *tar_file_start;
 
void initTarfs(){
    BINARY_TARFS_START = (uint64_t)&_binary_tarfs_start;
    BINARY_TARFS_END = (uint64_t)&_binary_tarfs_end;
    tar_file_start = (struct posix_header_ustar *)(BINARY_TARFS_START);
}

int strcmp(char *str1,char *str2){//returns 0 if equal, 1 str1>str2 else -1 if str1<str2
  int i=0;
  while(*(str1+i) != '\0' && *(str2+i) != '\0'){
    if(*(str1+i) == *(str2+i))
      i++;
    else{
      return *(str1+i) - *(str2+i);
    }
  }
  return *(str1+i) - *(str2+i);
}

void strcpy(char *src,char *dest){//Str Copy will be used only forcopying file names with less than 256 characters as of now,..
  int i=0;
  while(*(src+i) != '\0' && i<256){
    *(dest+i)=*(src+i);
    i++;
  }
  if(i==256){//TODO: Change this 256 if character count of a filename is increased.
    kprintf("FileName/DirName Exceeded 256 characters...My OS Doesnt allow you to do that :D\n");
  }
}

int startswith(char *parent,char * child){//returns 0 if child's name starts with parent's name.
  while(*parent!='\0'){
      if(*parent==*child){
          parent++;
          child++;
      }
      else{
          return -1;
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

void memcpy(void *src,void *dest,size_t n){//Doesnot care about source, copies from src to dest.
    unsigned char *s=src;
    unsigned char *d=dest;
    while(n>0){
      *d=*s;
      s++;
      d++;
      n--;
    }
}

int memread(void *src,void *dest,size_t n){//only copies data if NULL is not present in src
    unsigned char *s=src;
    unsigned char *d=dest;
    int y=n;
    while(n>0 && *s!='\0'){
      *d=*s;
      s++;
      d++;
      n--;
    }
    return y-n;
}

void memset_file(void *str, int c, size_t n){
    unsigned char *s = str;
    while(n > 0){
        *s = c;
        s++;
        n--;
    }
}

uint64_t map_elf_file(Elf64_Ehdr *elf_file,Elf64_Phdr *elf_p_hdr,uint64_t pml4,Task *uthread){
    uint64_t p_filesz=elf_p_hdr->p_filesz;
    uint64_t p_memsz=elf_p_hdr->p_memsz;
    uint64_t p_offset=elf_p_hdr->p_offset;
    uint64_t p_vaddr=elf_p_hdr->p_vaddr;
    //uint64_t p_paddr=elf_p_hdr->p_paddr; //TODO:Not sure which one to use between p_vaddr and p_paddr
    
    if(p_memsz<p_filesz){
        kprintf("MemSize of file is less than File Size.. Something wrong..\n");
        return 0;
    }
    
    //***********************Below code is to create VMA struct for each program header section.
    MM *mm = &uthread->memMap;
    VMA *vma_start;
    VMA *new_vma;
    vma_start = mm->mmap;
    while(vma_start != NULL && vma_start->next != NULL){
        vma_start=vma_start->next;
    }
    new_vma=(VMA *)kmalloc();//TODO: Allocating a full page for just one VMA struct, which is too expensive, SOL1: Can assume that one page is sufficient 
                             //for all the mallocs that will be done by the user. SOL2: Find a new way.. :P
    new_vma->pageNumber = getNextPageNum();
    if(vma_start==NULL){
        mm->mmap = new_vma;
    }
    else{
        vma_start->next=new_vma;
    }
    mm->count++;
    new_vma->v_file=(uint64_t)elf_file;
    new_vma->next=NULL;
    new_vma->v_start=p_vaddr;
    new_vma->v_end=p_vaddr+p_memsz;
    new_vma->v_flags=elf_p_hdr->p_flags;//TODO: So many other details can be stored in VMA struct
    
    //*************************END of creating VMA structs, Start of mapping memory..
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


uint64_t loadFile(char *file,uint64_t pml4,Task *uthread){
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
            if (!(map_elf_file(elf_file,elf_program_hdr+i,pml4,uthread))){
                kprintf("Error while loading program into memory..\n");
                return 0;
            }
        }
    }
    
    return elf_file->e_entry;
}

uint64_t get_file_address(char* file){
    uint64_t search_pointer = BINARY_TARFS_START;
    struct posix_header_ustar *tar_file_pointer = tar_file_start; 
    int file_found=0;
    while(search_pointer<BINARY_TARFS_END){
        if (strcmp(tar_file_pointer->name,"")!=0){
            //kprintf("    *  %s, size: %s\n",tar_file_pointer->name,tar_file_pointer->size);
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
    if(tar_file_pointer->size>0 && file_found==1){
        kprintf("File Found: %s\n");
        return (uint64_t)tar_file_pointer+512;
    }
    kprintf("Warning: File %s not found\n",file);
    return 0;
}

uint64_t get_dir_address(char* dir){
    uint64_t search_pointer = BINARY_TARFS_START;
    struct posix_header_ustar *tar_file_pointer = tar_file_start; 
    int dir_found=0;
    while(search_pointer<BINARY_TARFS_END){
        if (strcmp(tar_file_pointer->name,"\0")!=0){
            //kprintf("    *  %s, size: %s\n",tar_file_pointer->name,tar_file_pointer->size);
        }
        if (strcmp(tar_file_pointer->name,dir)==0){
            dir_found=1;
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
    char *f_size=tar_file_pointer->size;
    int size = toInteger(f_size);
    if(size==0 && dir_found==1){
        kprintf("Dir Found: %s\n");
        return (uint64_t)tar_file_pointer;
    }
    kprintf("Warning: Dir %s not found\n",dir);
    return 0;
}

struct fileDescriptor* get_fd_address(int fd){
    Task *currentTask=(Task *)getRunTask();
    int page=fd/5;
    int number=fd%5;
    uint64_t page_address = currentTask->fd_pointers[page];
    if(page_address!=0){
        struct fileDescriptor *fd_array=(struct fileDescriptor *)page_address;
        fd_array=fd_array+number;
        return fd_array;
    }
    kprintf("Looks Like an invalid FD..");
    return NULL;
}

int64_t openFile(char* file){
    Task *currentTask=(Task *)getRunTask();
    char newFileName[256];
    remove_dotslash(file,newFileName,0);
    char *fileName=newFileName+1;
    uint64_t start=get_file_address(fileName);
    if(start==0){
        kprintf("Invalid File Specified..\n");
        return -1;
    }
    for(int page=0;page<20;page++){
        uint64_t page_address = currentTask->fd_pointers[page];
        if(page_address==0){
            currentTask->fd_pointers[page]=(uint64_t)kmalloc();
            page_address=currentTask->fd_pointers[page];
            currentTask->fd_count+=5;
        }
        struct fileDescriptor *fd_array=(struct fileDescriptor *)page_address;
        for(int fd=0;fd<5;fd++){
            if(((5*page)+fd)>2){//checking if fd is not 0,1,2 because they are reserved.
                if(fd_array->name[0]=='\0'){
                   strcpy(fileName,fd_array->name);
                   fd_array->start=start;
                   fd_array->offset=start;
                   fd_array->d_prev_dir[0]='\0';
                   int file_fd = (5*page+fd);
                   kprintf("FD for above file: %d\n",file_fd);
                   return file_fd;
                   break; 
                }
            }
            fd_array++;
        }
    }
    kprintf("Trying to open more than 100 FD's..My OS allows user to open only 100 FD's per process.\n");
    return -1;
}

uint64_t readFile(int fd,char *buf,int count){
    if(fd<0){
        kprintf("Invalid FD provided..\n");
        return -1;
    }
    if(fd==0){
        char *dbuf=buf;
        *dbuf='0';
        
        while(count>0){
            *dbuf=readChar();
            if(*dbuf=='\0'){
              break;
            }
            else if(*dbuf=='\n'){
              *dbuf='\0';
              break;
            }
            dbuf++;
            count--;
        }
        return 0;
    }
    struct fileDescriptor *fd_struct=get_fd_address(fd);
    if(fd_struct==NULL){
        return -1;
    }
    uint64_t start;
    if(fd_struct->name[0]!= '\0'){
        start = fd_struct->offset;
    }
    else{
        kprintf("Incorrect File Descriptor..");
        return -1;
    }
    int val=memread((char *)start,buf,count);
    fd_struct->offset+=val;
    return val;
}

uint64_t closeFile(int fd){
    if(fd<0){
        kprintf("Invalid FD provided..\n");
        return 1;
    }
    struct fileDescriptor *fd_struct=get_fd_address(fd);
    if(fd_struct==NULL){
        return 1;
    }
    if(fd_struct->name[0]!='\0'){
        fd_struct->name[0]='\0';
    }
    else{
        kprintf("Err: Trying to close an unopened file\n");
    }
    return 0;
}

uint64_t writeFile(int fd,char *buf,int count){
    if(fd==1){
      kprintf("%s",buf);
      return count;
    }
    if(fd==2){
      kprintf("%s",buf);
      return count;//TODO: For now returning the input count, but not the actual count.
    }
    else{
      kprintf("Does not support write system call for now..");
      return 0;
    }
}

int64_t openDirectory(char* dir){
    Task *currentTask=(Task *)getRunTask();
    char newDirName[256];
    remove_dotslash(dir,newDirName,1);
    char *dirName=newDirName+1;
    uint64_t start=get_dir_address(dirName);
    if(start==0){
        kprintf("Invalid Directory Specified..\n");
        return -1;
    }
    for(int page=0;page<20;page++){
        uint64_t page_address = currentTask->fd_pointers[page];
        if(page_address==0){
            currentTask->fd_pointers[page]=(uint64_t)kmalloc();
            page_address=currentTask->fd_pointers[page];
            currentTask->fd_count+=5;
        }
        struct fileDescriptor *fd_array=(struct fileDescriptor *)page_address;
        for(int fd=0;fd<5;fd++){
            if(((5*page)+fd)>2){//checking if fd is not 0,1,2 because they are reserved.
                if(fd_array->name[0]=='\0'){
                   strcpy(dirName,fd_array->name);
                   fd_array->start=start;
                   fd_array->offset=start;
                   fd_array->d_prev_dir[0]='\0';
                   int dir_fd = (5*page+fd);
                   kprintf("FD for above file: %d\n",dir_fd);
                   return dir_fd;
                   break; 
                }
            }
            fd_array++;
        }
    }
    kprintf("Trying to open more than 100 FD's..My OS allows user to open only 100 FD's per process. :D\n");
    return -1;
}

int get_sub_directory(char *parent,char *child,char *dbuf,int count){
    char *buf=dbuf;
    while(*parent!='\0'){
      if(*parent==*child){
          parent++;
          child++;
      }
      else{
			*buf='\0';
			return -1;
      }
    }
    while((*child !='/') && (*child !='\0')){
        *buf=*child;
        child++;
        buf++;
		    count--;
		    if(count==0){
			    break;
		    }
    }
    if(count>0){
      if(*child!='\0'){
        if(*child=='/'){
          *buf='/';
          count--;
          buf++;
        }
      }
      if(count>0){
        *buf='\0';
      }
    }
	if(count!=0)
    buf='\0';
  return 0;
}

int get_sub_direntry(char *parent,char *child,char *dbuf,int count){
    char *buf=dbuf;
    while(*parent!='\0'){
      if(*parent==*child){
          parent++;
          child++;
      }
      else{
			*buf='\0';
			return -1;
      }
    }
    struct dirent *dir=(struct dirent*)buf;
    if(count<sizeof(struct dirent)){
      *buf='\0';
			return -1;
    }
    dir->d_reclen = sizeof(struct dirent);
    dir->d_ino = 0;//TODO: For Now.
    dir->d_off = 0;//TODO:No idea where it is being used..
    int i=0;
    while((*child !='/') && (*child !='\0')){
        dir->d_name[i]=*child;
        child++;
        i++;
		    count--;
		    if(count==0){
			    break;
		    }
    }
    if(i<255){
      if(*child!='\0'){
        if(*child=='/'){
          dir->d_name[i]='/';
          i++;
        }
      }
      if(i<255){
        dir->d_name[i]='\0';
      }
    }
  return 0;
}

struct dirent * readDir(int fd,char *buf,int count){
    if(fd<0){
        kprintf("Invalid FD provided..\n");
        return 0;
    }
    //Task *currentTask=(Task *)getRunTask();
    uint64_t start;
    struct posix_header_ustar *tar_file_pointer;
    struct fileDescriptor *fd_struct=get_fd_address(fd);
    struct dirent *dir=(struct dirent *)buf;
    if(fd_struct==NULL){
        return 0;
    }
    if(count<sizeof(struct dirent)){
      kprintf("Buffer size not sufficient for returning Dirent..");
      return 0;
    }
    if(fd_struct->name[0] != '\0'){
        start = fd_struct->offset;
        start = start+512;
        while(start<BINARY_TARFS_END){
          tar_file_pointer = (struct posix_header_ustar *)start;
          if(startswith(fd_struct->name,tar_file_pointer->name)==0){
              get_sub_directory(fd_struct->name,tar_file_pointer->name,dir->d_name,count);
              if((strcmp(dir->d_name,fd_struct->d_prev_dir))){
                  strcpy(dir->d_name,fd_struct->d_prev_dir);
                  fd_struct->offset=start;
                  dir->d_reclen = sizeof(struct dirent);
                  dir->d_ino = 0;//TODO: For Now.
                  dir->d_off = 0;
                  return dir;
              }
          }
		      else{
			      *buf='\0';
			      return 0;
		      }
          char *f_size=tar_file_pointer->size;
          int size = toInteger(f_size);
          int residue = size%512;
          if(residue>0){
            size=size+(512-residue);
          }
          start=start+size+512;
        }
    }
    kprintf("Incorrect FD...\n");
    return 0;
}

uint64_t closeDir(int fd){
    //Task *currentTask=(Task *)getRunTask();
    if(fd<0){
        kprintf("Invalid FD provided..\n");
        return 1;
    }
    struct fileDescriptor *fd_struct=get_fd_address(fd);
    if(fd_struct==NULL){
        return 1;
    }
    if(fd_struct->name[0]!='\0'){
        fd_struct->name[0]='\0';
    }
    else{
        kprintf("Err: Trying to close an unopened Directory FD\n");
    }
    return 0;
}

uint64_t getDirEntries(int fd,char *buf,int count){
    if(fd<0){
        kprintf("Invalid FD provided..\n");
        return 0;
    }
    int y=count;
    struct fileDescriptor *fd_struct=get_fd_address(fd);
    if(fd_struct==NULL){
        return 0;
    }
    //Task *currentTask=(Task *)getRunTask();
    uint64_t start;
    struct posix_header_ustar *tar_file_pointer;
    if(fd_struct->name[0] != '\0'){
        start = fd_struct->start;
        start = start+512;
        char prev_dir[NAME_MAX+1];
        prev_dir[0]='\0';
        struct dirent *dir;
        while(start<BINARY_TARFS_END){
          tar_file_pointer = (struct posix_header_ustar *)start;
          if(startswith(fd_struct->name,tar_file_pointer->name)==0){//if it starts with parent directory name then enter otherwise skip.
              int x = get_sub_direntry(fd_struct->name,tar_file_pointer->name,buf,count);
              if(x==0){
                dir=(struct dirent *)buf;
                if((strcmp(dir->d_name,prev_dir))){//checking if the prev_dir is the same as cur_dir
                  strcpy(dir->d_name,prev_dir);
                  buf+=sizeof(Dirent); //if yes then keeping that direntry in the output
                  count-=sizeof(Dirent);
                }
              }
          }
		      else{//if not as parent.. buf will be pointing to end of direntries thus writing a null there and returning 0.
			      *buf='\0';
			      return y-count;
		      }
          char *f_size=tar_file_pointer->size;//if you got a child dir, then check if there are any other entries.
          int size = toInteger(f_size);
          int residue = size%512;
          if(residue>0){
            size=size+(512-residue);
          }
          start=start+size+512;
        }
    }
  *buf='\0';
  return 0;
}

char *getCWD(char *dest,int count){
    char *environ_main=(char **)0x1000;//WARNING: ENVP IS HARDCODED to 0.. So dont try to change it.. TODO:
    char *buf=dest;
    int env_i =0;
    char temp[256];
    int arg_i=0;
    while(*(environ_main+env_i) != '\0'){
      char *env_entry=environ_main+env_i;
      while(env_entry[arg_i] != '='){
        temp[arg_i]=env_entry[arg_i];
        arg_i++;
      }
      temp[arg_i]='\0';
      arg_i++;
      if(strcmp(temp,"PWD") == 0){
        int start = arg_i;
        while(env_entry[start] != '\0'){
          *buf=env_entry[start];
          start++;
          buf++;
          count--;
          if(count==0){
            break;
          }
        }
        if(count>1){
          *buf='\0';
        }
      }
      env_i+=0x1000;
  }
  return ((char *)env_entry+arg_i);
}

void get_between_2slashes(char *src,char *dest){
    char *input=src;
    char *output=dest;
    if(*input=='/'){
        *output='\0';
    }
    while(*input!='\0' || *input!='/'){
        *output=*input;
        output++;
        input++;
    }
}

void remove_dotslash(char *src,char *dest,int d){
    char temp_output[256];
    temp_output[0]='\0';
    char *input=src;
    char *output=dest;
    if(*input=='/'){
        *output='/';
        input++;
    }
    else{
      char cwd[256];
      getCWD(cwd,256);
      int i=0;
      while(cwd[i]!='\0'){
        *output=cwd[i];
        output++;
        i++;
      }
      if(*(output-1)!='/'){
        *output='/';
        output++;
      }
    }
    while(*input!='\0'){
      get_between_2slashes(input,temp_output);
      
      if(!strcmp(temp_output,".")){
          if(*(input+1)=='/'){
            input+=2;
          }
          else{
            input+=1;
          }
      }
      else if(!strcmp(temp_output,"\0")){
          input+=1;
      }
      else if(!strcmp(temp_output,"..")){
          if(*(input+2)=='/'){
            input+=3;
          }
          else{
            input+=2;
          }
          output--;
          if(output==dest){
            output++;
          }
          else{
            output--;
            while(*output!='/'){
              output--;
            }
            output++;
          }
      }
      else{
        int i=0;
        while(temp_output[i]!='\0'){
          *output=*temp_output;
          output++;
          i++;
          input++;
        }
        if(*input=='/'){
          *output='/';
          output++;
        }
      }
    }
    if(d==1){
        if(*(output-1)!='/'){
          *output='/';
          output++;
        }
    }
    *output='\0';
}

int check_if_directory_present(char *buf){
    int i=get_dir_address(buf);
    if(i>0){
      return 1;
    }
    return 0;
}

int64_t changeDirectory(char *buf){
    char *src=buf;
    char dest[256];
    remove_dotslash(src,dest,1);
    if(!check_if_directory_present(dest+1)){
      kprintf(" %s Directory Not Found..",buf);
      return -1;
    }
    else{
      char test[256];
      char *env_value=getCWD(test,256);
      while(*buf!='\0'){
        *env_value=*buf;
        env_value++;
        buf++;
      }
      return 0;
    }
    return -1;
}
/*void full_path_builder(char *input,char *output){
    char temp_output[256];
    remove_dotslash(char *input,char *temp_output);
    if(*input=='/'){
        *output=*input;
        input++;
        output++;
        while(*input!='/'){
            *output=*input;
        }
    
    
    }
    else{
    
    }
}*/
