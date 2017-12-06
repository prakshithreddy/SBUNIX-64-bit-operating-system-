#include <stdlib.h>
#include <unistd.h>
#include<fcntl.h>


void* syscall(void* syscallNum,void* param1,void* param2,void* param3,void* param4,void* param5,void* param6) {
    void* returnValue;
    __asm__ volatile ("movq %1, %%rax; movq %2, %%rdi; movq %3, %%rsi; movq %4, %%rdx; movq %5, %%r10; movq %6, %%r8;movq %7, %%r9;syscall;movq %%rax, %0;":"=g"(returnValue):"g"(syscallNum),"g"(param1),"g"(param2),"g"(param3),"g"(param4),"g"(param5),"g"(param6):"%rax","%rdi","%rsi","%rdx","%r10","%r8","%r9","memory");//clobber list
    return returnValue;
}

void puts(char *buf){
  int count=0;
  while(buf[count]!='\0') count++;
  syscall((void*)(uint64_t)4,(void*)(uint64_t)1,(void*)buf,(void*)(uint64_t)count,0,0,0);
}

char getChar(){
    char c;
    syscall((void*)(uint64_t)0,(void*)(uint64_t)0,(void*)&c,(void*)(uint64_t)1,0,0,0);
    return c;
}

uint64_t malloc(uint64_t size)
{
    return (uint64_t)syscall((void*)(uint64_t)99,(void*)size,0,0,0,0,0);
}

int main(int argc, char *argv[], char *envp[]){

    char *ps1="sbush>";
    puts(ps1);
    
    char *inputStr = (char*)malloc(2000);
    int i=0;
    
    while(1)
    {
        char c = getChar();
        if(c =='\0') break;
        else if(c==' ' && i==0) continue;
        else if(i>2000) break;
        else if(c != NULL)
        {
            inputStr[i]=c;
            i++;
        }
        
        
    }
    inputStr[i]='\0';
    
    puts(inputStr);
    
    
    
    //while(1);
}
