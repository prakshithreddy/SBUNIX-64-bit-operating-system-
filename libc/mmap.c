#include<sys/mman.h>
#include<sys/syscall.h>
#include<sys/defs.h>



void *mmap(void *addr, size_t length, int prot, int flags,int fd, off_t offset){
    
   // return (void*)syscall5((void*)9,NULL,(void*)4096,(void*)PROT_NONE,(void*)(MAP_PRIVATE|MAP_ANONYMOUS),0,0);
    return (void*)syscall5((void*)9,(void*)addr,(void*)(ssize_t) length,(void*)(ssize_t) prot,(void*)(ssize_t) flags,0);
}


int munmap(void *addr, size_t length)
{
    return (ssize_t)syscall5((void*)11,(void*)addr,(void*)(ssize_t) length,0,0,0);
}
