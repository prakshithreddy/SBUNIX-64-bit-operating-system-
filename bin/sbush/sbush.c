#include <stdlib.h>

int main(int argc, char *argv[], char *envp[]){
    argc+=1;
    syscall(argc,1,2,3,4,5,6);
    //kprintf("%d",retVal);
        while(1);
}
