
#include <stdlib.h>


void* syscall(void* syscallNum,void* param1,void* param2,void* param3,void* param4,void* param5,void* param6) {
    void* returnValue;
    __asm__ volatile ("movq %1, %%rax; movq %2, %%rdi; movq %3, %%rsi; movq %4, %%rdx; movq %5, %%r10; movq %6, %%r8;movq %7, %%r9;syscall;movq %%rax, %0;":"=g"(returnValue):"g"(syscallNum),"g"(param1),"g"(param2),"g"(param3),"g"(param4),"g"(param5),"g"(param6):"%rax","%rdi","%rsi","%rdx","%r10","%r8","%r9","memory");//clobber list
    return returnValue;
}

int main(int argc, char *argv[], char *envp[]){
    
    //syscall((void*)44,(void*)2,(void*)&a1[0],(void*)3,(void*)4,(void*)5,(void*)a1);
    return 0;
}