#include<stdlib.h>
#include<sys/defs.h>
#include<sys/syscall.h>

//uint64_t malloc(ssize_t size);

//void *malloc(ssize_t size);
void * malloc(ssize_t size)
{
    return (void *)syscall((void*)(uint64_t)99,(void*)size,0,0,0,0,0);
}

void free(void *ptr){
    syscall((void*)(uint64_t)89,(void*)ptr,0,0,0,0,0);
}

void exit(int rc){
    syscall((void*)(uint64_t)100,(void*)(int64_t)rc,0,0,0,0,0);
}

int64_t setenv(char *name,char *value, int overwrite){
    return (int64_t)syscall((void*)(uint64_t)53,(void*)name,(void *)value,(void *)(uint64_t)overwrite,0,0,0);
}

char *getenv(char *name){
    return (char *)syscall((void*)(uint64_t)54,(void*)name,0,0,0,0,0);
}
int64_t env(char **com_args){
    return (int64_t)syscall((void*)(uint64_t)55,(void*)com_args,0,0,0,0,0);
}