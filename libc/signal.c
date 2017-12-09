#include<signal.h>
#include <sys/defs.h>
#include<sys/syscall.h>

int kill(int64_t pid, int sig){
    syscall((void*)199,(void*)pid,(void*)0,(void*)3,(void*)4,(void*)5,(void*)0);
    return 0;
}

sighandler_t signal(int signum, sighandler_t handler){
    return (sighandler_t)syscall((void*)155,(void*)(int64_t)signum,(void*)handler,(void*)3,(void*)4,(void*)5,(void*)0);
}

int alarm(int seconds){
    return (int64_t)syscall((void*)156,(void*)(int64_t)seconds,0,(void*)3,(void*)4,(void*)5,(void*)0);
}