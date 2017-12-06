#include <stdlib.h>
#include <unistd.h>
#include<fcntl.h>


void* syscall(void* syscallNum,void* param1,void* param2,void* param3,void* param4,void* param5,void* param6) {
    void* returnValue;
    __asm__ volatile ("movq %1, %%rax; movq %2, %%rdi; movq %3, %%rsi; movq %4, %%rdx; movq %5, %%r10; movq %6, %%r8;movq %7, %%r9;syscall;movq %%rax, %0;":"=g"(returnValue):"g"(syscallNum),"g"(param1),"g"(param2),"g"(param3),"g"(param4),"g"(param5),"g"(param6):"%rax","%rdi","%rsi","%rdx","%r10","%r8","%r9","memory");//clobber list
    return returnValue;
}

int main(int argc, char *argv[], char *envp[]){

    int* a1 = (char *)syscall((void*)99,(void*)2000,(void*)0,(void*)0,(void*)0,(void*)0,(void*)0);
    a1[0] = 10;
    uint64_t pid = (uint64_t)syscall((void*)1,(void*)1,(void*)2,(void*)3,(void*)4,(void*)5,(void*)a1);
    
    if(pid == 0)
    {
        a1[3] = 786;
        
        char* temp = "bin/ls";
        
        syscall((void*)78,(void*)temp,(void*)argv,(void*)envp,(void*)1,(void*)2,(void*)3);
        //syscall(10,(uint64_t)a,a1[0],a1[1],a1[3],0,0);
        //char *c=(char *)syscall(99,2000,0,0,0,0,0);
        // *c='\0';
        //while(1){
            //syscall(5,0,(uint64_t)c,64,0,0,0);
            //syscall(6,1,(uint64_t)c,0,0,0,0);
        //}
//        char** a = (char**)syscall((void*)99,(void*)(4*sizeof(char*)),(void*)0,(void*)0,(void*)0,(void*)0,(void*)0);
//
//        //for(int i=0;i<10;i++) a[i] = (char*)syscall((void*)99,(void*)(uint64_t)(10*sizeof(char)),(void*)0,(void*)0,(void*)0,(void*)0,(void*)0);
//
//        for(int i=0;i<4;i++) a[i] = (char*)"a=kjhs";
//        a[3] = (char*)"a=kjhsqqqqqqkajhskjdasd";
        //char* temp = (char*)"bin/ls";
        
       // syscall((void*)420,(void*)(uint64_t)argc,(void*)argv,(void*)envp,(void*)0,(void*)(uint64_t)4,(void*)5);
        
       // syscall((void*)78,(void*)temp,(void*)argv,(void*)envp,(void*)1,(void*)2,(void*)3);
        while(1);// syscall((void*)10,(void*)a,(void*)(uint64_t)a1[0],(void*)(uint64_t)a1[1],(void*)(uint64_t)a1[3],(void*)0,(void*)(uint64_t)argc);
    }
    else
    {
        syscall((void*)88,(void*)2,(void*)&a1[0],(void*)3,(void*)4,(void*)5,(void*)a1);
        
    }
///<<<<<<< HEAD
//    {*/
//          //a1[1] = 11;
//        char *f="test.txt";
//        int fd = open(f,1);
//        if(fd!=-1){
//          read(fd,a1,5);
//          write(1,a1,5);
//          close(fd);
//        }
//        char* a2 = (char *)syscall((void*)99,(void*)2000,(void*)0,(void*)0,(void*)0,(void*)0,(void*)0);
//        getcwd(a2,20);
//        write(1,a2,20);
//        chdir("/bin");
//        getcwd(a2,20);
//        write(1,a2,20);
//        chdir("lakalak");
//        getcwd(a2,20);
//        write(1,a2,20);
//        chdir("../");
//        getcwd(a2,20);
//        write(1,a2,20);
//        chdir("./lakalak/../lakalak/");
//        getcwd(a2,20);
//        write(1,a2,20);
//        chdir("/usr/folder1/folder2/..");
//        getcwd(a2,20);
//        write(1,a2,20);
//        //int64_t i = (int64_t)syscall((void*)88,(void*)2,(void*)&a1[0],(void*)3,(void*)4,(void*)5,(void*)a1);
//        //syscall((void*)420,(void*)envp,(void*)0,(void*)0,(void*)(uint64_t)4,(void*)5,(void*)(int64_t)i);
////=======
//
//          a1[1] = 11;
//        char* temp = (char*)"bin/ls";
//        //asdasd
//       syscall((void*)88,(void*)2,(void*)&a1[0],(void*)3,(void*)4,(void*)5,(void*)a1);
//      //  syscall((void*)420,(void*)(uint64_t)argc,(void*)argv,(void*)envp,(void*)0,(void*)(uint64_t)4,(void*)5);//,(void*)(int64_t)i);
//
////>>>>>>> c3037649dc43c2190cb8ee587a4268edaf2260c9
//        while(1);// syscall((void*)20,(void*)(uint64_t)a1[0],(void*)2,(void*)(uint64_t)a1[1],(void*)(uint64_t)a1[3],(void*)5,(void*)(uint64_t)argc);
//    //}
//
//    //syscall(10,1,2,3,4,5,6);
//    //syscall(11,1,2,3,4,5,6);
////    king fine.... need to debug the mischevious child//
////    //kprintf("%d",retVal);
////    syscall(argc,1,2,3,4,5,6);
////    syscall(argc,1,2,3,4,5,6);
////
////    char *s = (void*)(0x0);
////    //kprintf("%p",s);
////    *s='a';
////
////    syscall(argc,1,2,3,4,5,6);
     while(1);
}
