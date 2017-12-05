#include <stdlib.h>


void* syscall(void* syscallNum,void* param1,void* param2,void* param3,void* param4,void* param5,void* param6) {
    void* returnValue;
    __asm__ volatile ("movq %1, %%rax; movq %2, %%rdi; movq %3, %%rsi; movq %4, %%rdx; movq %5, %%r10; movq %6, %%r8;movq %7, %%r9;syscall;movq %%rax, %0;":"=g"(returnValue):"g"(syscallNum),"g"(param1),"g"(param2),"g"(param3),"g"(param4),"g"(param5),"g"(param6):"%rax","%rdi","%rsi","%rdx","%r10","%r8","%r9","memory");//clobber list
    return returnValue;
}

int main(int argc, char *argv[], char *envp[]){
    
//    int i=0;
    //syscall((void*)420,(void*)(uint64_t)argc,(void*)argv,(void*)envp,(void*)(uint64_t)4,(void*)5,(void*)6);
    
//    while(argv[i][0]!='\0')
//    {
//        syscall((void*)54,(void*)(uint64_t)argc,(void*)argv[i],(void*)(uint64_t)envp,(void*)(uint64_t)4,(void*)5,(void*)6);
//        i+=1;
//    }
//    i=0;
//    while(envp[i][0]!='\0')
//    {
//        syscall((void*)54,(void*)(uint64_t)argc,(void*)envp[i],(void*)(uint64_t)envp,(void*)(uint64_t)4,(void*)5,(void*)6);
//        i+=1;
//    }
    syscall((void*)420,(void*)argv,(void*)0,(void*)0,(void*)(uint64_t)4,(void*)5,(void*)(int64_t)2);
    while(1);//syscall((void*)54,(void*)(uint64_t)argc,(void*)argv,(void*)(uint64_t)envp,(void*)(uint64_t)4,(void*)5,(void*)6);
}
