#include <sys/defs.h>
#include <unistd.h>
#include <sys/syscall.h>

ssize_t write(int fd, const void *buf, size_t count){
    return (ssize_t)syscall5 ((void*)4,(void*)(ssize_t)fd,(void*)buf,(void*)count,0,0);
}


ssize_t read(int fd, void *buf, size_t count)
{
    return (ssize_t)syscall5 ((void*)0,(void*)(ssize_t)fd,(void*)buf,(void*)count,0,0);
    
}

pid_t fork()
{
    return (ssize_t)syscall5 ((void*)57,0,0,0,0,0);
}

int dup(int oldfd)
{
    return (ssize_t)syscall5 ((void*)32,(void*)(ssize_t)oldfd,0,0,0,0);
    
}

int execve(const char *file, char *const argv[], char *const envp[])
{
    return (ssize_t)syscall5 ((void*)59,(void*)file,(void*)argv,(void*)envp,0,0);
    
}

int pipe(int pipefd[2])
{
    return (ssize_t) syscall5((void*)22,(void*)(ssize_t)pipefd,0,0,0,0);
    
}

int close(int fd)
{
    return (ssize_t) syscall5((void*)3,(void*)(ssize_t)fd,0,0,0,0);
    
}

char *getcwd(char *buf, size_t size){
    
    return (char*) syscall5((void*)79,(void*)buf,(void*)(ssize_t) size,0,0,0);
    
}

