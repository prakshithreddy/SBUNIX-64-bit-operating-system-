#include <stdlib.h>


uint64_t syscall(uint64_t syscallNum,uint64_t param1,uint64_t param2,uint64_t param3,uint64_t param4,uint64_t param5,uint64_t param6) {
    uint64_t returnValue;
    __asm__ volatile ("movq %1, %%rax; movq %2, %%rdi; movq %3, %%rsi; movq %4, %%rdx; movq %5, %%r10; movq %6, %%r8;movq %7, %%r9;syscall;movq %%rax, %0;":"=g"(returnValue):"g"(syscallNum),"g"(param1),"g"(param2),"g"(param3),"g"(param4),"g"(param5),"g"(param6):"%rax","%rdi","%rsi","%rdx","%r10","%r8","%r9","memory");//clobber list
    return returnValue;
}

int main(int argc, char *argv[], char *envp[]){
    argc+=1;
    syscall(argc,1,2,3,4,5,6);
    //kprintf("%d",retVal);
    syscall(argc,1,2,3,4,5,6);
    syscall(argc,1,2,3,4,5,6);
    
    char *s = (void*)(0x0);
    //kprintf("%p",s);
    *s='a';

    syscall(argc,1,2,3,4,5,6);
        while(1);
}