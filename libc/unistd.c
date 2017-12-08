#include<unistd.h>
#include<sys/defs.h>
#include<sys/syscall.h>

int open(const char *pathname, int flags){
    return (ssize_t)syscall((void*)2,(void*)pathname,(void*)(ssize_t)flags,0,0,0,0);
}

int close(int fd)
{
    return (ssize_t) syscall((void*)3,(void*)(uint64_t)fd,0,0,0,0,0);
}

ssize_t write(int fd, const void *buf, size_t count){
    return (ssize_t)syscall((void*)4,(void*)(ssize_t)fd,(void*)buf,(void*)count,0,0,0);
}

ssize_t read(int fd, void *buf, size_t count)
{
    return (ssize_t)syscall((void*)0,(void*)(ssize_t)fd,(void*)buf,(void*)count,0,0,0);
}

pid_t fork(){
    return (uint64_t)syscall((void*)(uint64_t)1,0,0,0,0,0,0);
}

pid_t getpid(){
    return (uint64_t)syscall((void*)(uint64_t)66,0,0,0,0,0,0);
}

pid_t getppid(){
    return (uint64_t)syscall((void*)(uint64_t)67,0,0,0,0,0,0);
}

int execve(const char *file, char *const argv[], char *const envp[])
{
    return (ssize_t)syscall((void*)78,(void*)file,(void*)argv,(void*)envp,0,0,0);
}

int execvpe(const char *file, char *const argv[], char *const envp[])
{
    return (ssize_t)syscall((void*)78,(void*)file,(void*)argv,(void*)envp,0,0,0);
}

int waitpid(int pid, int *status,int options)
{
    return (ssize_t)syscall((void*)88,(void*)(uint64_t)pid,(void *)status,0,0,0,0);
}

int chdir(const char *path)
{
  return (ssize_t)syscall((void*)80,(void*)path,0,0,0,0,0);
}

char *getcwd(char *buf, size_t size){
  return (char *)syscall((void*)79,(void *)buf,(void *)(uint64_t)size,0,0,0,0);
}

unsigned int sleep(unsigned int seconds){
  
  syscall((void*)50,(void*)0,(void*)0,(void*)0,(void*)0,(void*)0,(void*)0);
    
  while((uint64_t)syscall((void*)51,(void*)0,(void*)0,(void*)0,(void*)0,(void*)0,(void*)0)<=seconds);
    
  syscall((void*)52,(void*)0,(void*)0,(void*)0,(void*)0,(void*)0,(void*)0);
}
pid_t wait(int *status);
