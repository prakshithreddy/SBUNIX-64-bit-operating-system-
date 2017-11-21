#include<sys/syscall.h>
#include<sys/defs.h>
#include<fcntl.h>

int open(const char *pathname, int flags){
    return (ssize_t)syscall5((void*)2,(void*)pathname,(void*)(ssize_t)flags,0,0,0);
    
}

