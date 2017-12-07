#include <stdlib.h>
#include<sys/defs.h>

#define null (void*)0


void* syscall1(void* syscallNum,void* param1,void* param2,void* param3,void* param4,void* param5,void* param6) {
    void* returnValue;
    __asm__ volatile ("movq %1, %%rax; movq %2, %%rdi; movq %3, %%rsi; movq %4, %%rdx; movq %5, %%r10; movq %6, %%r8;movq %7, %%r9;syscall;movq %%rax, %0;":"=g"(returnValue):"g"(syscallNum),"g"(param1),"g"(param2),"g"(param3),"g"(param4),"g"(param5),"g"(param6):"%rax","%rdi","%rsi","%rdx","%r10","%r8","%r9","memory");//clobber list
    return returnValue;
}


void clrScreen()
{
    syscall1((void*)(uint64_t)123,0,0,0,0,0,0);
}

void _start(void) {
  // call main() and exit() here
    uint64_t* rsp;
    
    __asm__ __volatile__("movq %%rsp,%0;":"=r"(rsp)::);
    
    rsp = (uint64_t*)((char*)(*(rsp+1)));
    clrScreen();
    main(*(rsp+1),(char**)(rsp+2),(char**)(rsp+*(rsp+1)+3));

    syscall1((void*)(uint64_t)100,0,0,0,0,0,0);
    
    //while(1);
    
}

