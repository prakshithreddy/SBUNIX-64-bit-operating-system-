#include <stdlib.h>

#define ARGUMENT_BUFFER 32
int pip_flag = 0;

void* syscall(void* syscallNum,void* param1,void* param2,void* param3,void* param4,void* param5,void* param6) {
    void* returnValue;
    __asm__ volatile ("movq %1, %%rax; movq %2, %%rdi; movq %3, %%rsi; movq %4, %%rdx; movq %5, %%r10; movq %6, %%r8;movq %7, %%r9;syscall;movq %%rax, %0;":"=g"(returnValue):"g"(syscallNum),"g"(param1),"g"(param2),"g"(param3),"g"(param4),"g"(param5),"g"(param6):"%rax","%rdi","%rsi","%rdx","%r10","%r8","%r9","memory");//clobber list
    return returnValue;
}

char **parse_command(char *command){
    char **com_args = (char **)malloc(ARGUMENT_BUFFER*sizeof(char*));
    int buffer = ARGUMENT_BUFFER;
    
    if(!com_args){
        exit(1);
    }
    
    int i,start,end,n;
    
    i=start=end=n=0;
    
    if (*command == '\0')
        return NULL;
    
    while(*(command+i) != '\0'){
        if(n>buffer){
            buffer = buffer+ARGUMENT_BUFFER;
        }
        if(command[i] != ' ' && command[i] != '\t' && command[i] != '\a' && command[i] != '\r' && command[i] != '\n'){
            end = i;
        }
        else{
            if((start>end) && (command[i] == ' ' || command[i] == '\t' || command[i] == '\a' || command[i] == '\r' || command[i] == '\n')){
                start++;
            }
            else if(end == 0)
                start++;
            else{
                //printf("Tokenizing");
                //printf("Command before tokenizing: %s\n",command);
                if(strcmp(com_args[n++] = token(command,start,end),"|") == 0){
                    pipe_flag = 1 ;
                    //printf("Pipe flag is set to true");
                }
                start=i+1;
                //printf("After Tokeninzing");
            }
        }
        i++;
    }
    if(start<=end){
        //printf("n before last arg: %d",n);
        //com_args[n] = token(command,start,end);
        //printf("n after com_args: %d, com_args[n] at last = %s",n,com_args[n]);
        if(strcmp(com_args[n] = token(command,start,end),"|") == 0 ){
            pipe_flag = 1;
        }
    }
    else{
        n--;
    }
    //printf("here1");
    if(strcmp(com_args[n],"&") == 0){
        background = 1;
        //printf("here2");
        com_args[n] = '\0';
    }
    else{
        //printf("here3");
        n++;
        com_args[n] = '\0';
    }
    return com_args;
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

//    char *ps1="sbush>";
//    puts(ps1);
    
    while(1)
    {
        //read a input line
        char *inputStr = (char*)malloc(2000);
        
        int i=0;
        
        while(1)
        {
            char c = getChar();
            if(c =='\0' && i>0) break;
            else if(c==' ' && i==0) continue;
            else if(i>2000) break;
            else if(c != '\0')
            {
                inputStr[i]=c;
                i++;
            }
            
            
        }
        
        inputStr[i]='\0';
        //end of readline
        
        //parse the command
        
        
        
    }
    
    puts(inputStr);
    
    
    
    while(1);
}
