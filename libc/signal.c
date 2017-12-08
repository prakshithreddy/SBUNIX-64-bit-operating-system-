#include<signal.h>
#include <sys/defs.h>
#include<sys/syscall.h>

int kill(int64_t pid, int sig){
    syscall((void*)199,(void*)pid,(void*)0,(void*)3,(void*)4,(void*)5,(void*)0);
    return 0;
}