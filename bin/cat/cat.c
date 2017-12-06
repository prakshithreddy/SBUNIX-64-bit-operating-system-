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

int open(const char *pathname, int flags){
    return (ssize_t)syscall((void*)2,(void*)pathname,(void*)(ssize_t)flags,0,0,0,0);
    
}

ssize_t write(int fd, const void *buf, size_t count){
    return (ssize_t)syscall((void*)4,(void*)(ssize_t)fd,(void*)buf,(void*)count,0,0,0);
}

ssize_t read(int fd, void *buf, size_t count)
{
    return (ssize_t)syscall((void*)0,(void*)(ssize_t)fd,(void*)buf,(void*)count,0,0,0);
    
}

int main(int argc, char*argv[],char* envp[])
{
    
    //struct stat* temp = (struct stat*) malloc(sizeof(struct stat));
    
    //stat(argv[1],temp);
    
    int fd = open(argv[1], O_RDONLY);
    if(fd==-1){
        write(1,"cat: ",10);
        write(1,argv[1],256);
        write(1," File Not Found..",10);
        return 0;
    }
    int l_cou=1;
    while(l_cou!=0){
    
    char* cat = (char*) malloc(4096);
    
    l_cou = read(fd,cat,4096);
    
    write(1,cat,l_cou);
    
    }
    write(1,"\n",1);
    
    return 0;
}
