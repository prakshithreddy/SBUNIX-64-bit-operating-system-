#include <stdlib.h>


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
    syscall((void*)420,(void*)(uint64_t)argc,(void*)argv,(void*)envp,(void*)(uint64_t)4,(void*)5,(void*)(int64_t)2);
    while(1);//syscall((void*)54,(void*)(uint64_t)argc,(void*)argv,(void*)(uint64_t)envp,(void*)(uint64_t)4,(void*)5,(void*)6);
}
