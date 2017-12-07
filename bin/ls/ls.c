#include <stdlib.h>
#include <dirent.h>
#include <sys/defs.h>



void* syscall(void* syscallNum,void* param1,void* param2,void* param3,void* param4,void* param5,void* param6) {
    void* returnValue;
    __asm__ volatile ("movq %1, %%rax; movq %2, %%rdi; movq %3, %%rsi; movq %4, %%rdx; movq %5, %%r10; movq %6, %%r8;movq %7, %%r9;syscall;movq %%rax, %0;":"=g"(returnValue):"g"(syscallNum),"g"(param1),"g"(param2),"g"(param3),"g"(param4),"g"(param5),"g"(param6):"%rax","%rdi","%rsi","%rdx","%r10","%r8","%r9","memory");//clobber list
    return returnValue;
}


uint64_t malloc(uint64_t size)
{
    return (uint64_t)syscall((void*)(uint64_t)99,(void*)size,0,0,0,0,0);
}

char *getcwd(char *buf, size_t size){
    
    return (char*) syscall((void*)79,(void*)buf,(void*)(ssize_t) size,0,0,0,0);
    
}

int open(const char *pathname, int flags){
    return (ssize_t)syscall((void*)9,(void*)pathname,(void*)(ssize_t)flags,0,0,0,0);
    
}

int getdents(unsigned int fd, char *dirp,unsigned int count)
{
    return (ssize_t)syscall((void*)12,(void*)(ssize_t)fd,(void*)dirp,(void*)(ssize_t)count,0,0,0);
}

ssize_t write(int fd, const void *buf, size_t count){
    return (ssize_t)syscall((void*)4,(void*)(ssize_t)fd,(void*)buf,(void*)count,0,0,0);
}

int main(int argc,char* argv[],char* envp[])
{
    
        char* file_path;
    
    if(argc==1){
    
        file_path = (char*)malloc(4096);
        getcwd(file_path,1024);
        
    
    }
    else file_path = argv[1];
    
    struct dirent* temp;
    
    char* dirp = (char*)malloc(4096);
    
    int fd = open(file_path,O_RDONLY|O_DIRECTORY);
    
    int n = getdents(fd,dirp,4096);
    
    for( int i=0;i<n;)
    {
        temp = (struct dirent*) (dirp+i);

        
        if(temp->d_name[0]=='.') {
            i+=temp->d_reclen; continue;
        }
        
        int k=0;
        
        while(temp->d_name[k]!='\0')
        {
            k++;
        }
        
        write(1,temp->d_name,k);
        
        /*int d_type = *(dirp + i + temp->d_reclen - 1);
        
        if(d_type == DT_DIR)
        {
            write(1,"/",1);
            
        }*/
        
        write(1,"  ",1);
        
        
        i+=temp->d_reclen;
    }
    write(1,"\n",1);
    
    //struct stat* temp = (struct stat*) mmap(NULL,sizeof(struct stat),PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE,-1,0);
    
    // stat(path,temp);
    
    
    
    return 0;
    
    
}

