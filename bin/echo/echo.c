#include<stdlib.h>

void* syscall(void* syscallNum,void* param1,void* param2,void* param3,void* param4,void* param5,void* param6) {
    void* returnValue;
    __asm__ volatile ("movq %1, %%rax; movq %2, %%rdi; movq %3, %%rsi; movq %4, %%rdx; movq %5, %%r10; movq %6, %%r8;movq %7, %%r9;syscall;movq %%rax, %0;":"=g"(returnValue):"g"(syscallNum),"g"(param1),"g"(param2),"g"(param3),"g"(param4),"g"(param5),"g"(param6):"%rax","%rdi","%rsi","%rdx","%r10","%r8","%r9","memory");//clobber list
    return returnValue;
}

ssize_t write(int fd, const void *buf, size_t count){
    return (ssize_t)syscall((void*)4,(void*)(ssize_t)fd,(void*)buf,(void*)count,0,0,0);
}


int main(int argc,char* argv[],char* envp[])
{
    if(argc==1){
        return 0;
    }
    else if(argc>1){
        int i=1;
        while(argv[i]!='\0'){
            write(1,argv[i],512);
            write(1," ",512);
            i++;
        }
    }
    return 0;
}