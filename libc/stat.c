#include<stat.h>
#include<sys/syscall.h>
#include<sys/defs.h>

int stat(const char *path, struct stat *buf)
{
    return (ssize_t)syscall5((void*)4,(void*)path,(void*)buf,0,0,0);
}
