#include <sys/defs.h>
#include <sys/wait.h>
#include <sys/syscall.h>

int wait4(pid_t id, int *status,int options,void* rusage)
{
    return (ssize_t) syscall5((void*)61,(void*)(ssize_t)id,(void*)(ssize_t)status,(void*)(ssize_t)options,(void*)rusage,0);
}

int waitpid(int pid, int *status,int options)
{
    return wait4(pid,status,options,NULL);
}