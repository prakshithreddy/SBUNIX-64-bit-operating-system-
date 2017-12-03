#include <stdlib.h>


void* syscall(uint64_t syscallNum,uint64_t param1,uint64_t param2,uint64_t param3,uint64_t param4,uint64_t param5,uint64_t param6) {
    void* returnValue;
    __asm__ volatile ("movq %1, %%rax; movq %2, %%rdi; movq %3, %%rsi; movq %4, %%rdx; movq %5, %%r10; movq %6, %%r8;movq %7, %%r9;syscall;movq %%rax, %0;":"=g"(returnValue):"g"(syscallNum),"g"(param1),"g"(param2),"g"(param3),"g"(param4),"g"(param5),"g"(param6):"%rax","%rdi","%rsi","%rdx","%r10","%r8","%r9","memory");//clobber list
    return returnValue;
}

int main(int argc, char *argv[], char *envp[]){
    argc+=1;
    
    int* a1 = (int*)syscall(99,4096,0,0,0,0,0);
    a1[0] = 10;
    a1[0] = 10;
    uint64_t pid = (uint64_t)syscall(1,1,2,3,4,5,(uint64_t)a1);
    
    if(pid == 0)
    {

        int* a = (int*)syscall(99,4096,0,0,0,0,0);
        a1[3] = 786;
        while(1)syscall(10,(uint64_t)a,a1[0],a1[1],a1[3],0,0);
    }
    else
    {
          a1[1] = 11;
        while(1)syscall(20,a1[0],2,a1[1],4,5,6);
    }
    
    //syscall(10,1,2,3,4,5,6);
    //syscall(11,1,2,3,4,5,6);
//    king fine.... need to debug the mischevious child//
//    //kprintf("%d",retVal);
//    syscall(argc,1,2,3,4,5,6);
//    syscall(argc,1,2,3,4,5,6);
//    
//    char *s = (void*)(0x0);
//    //kprintf("%p",s);
//    *s='a';
//    
//    syscall(argc,1,2,3,4,5,6);
     while(1);
}
