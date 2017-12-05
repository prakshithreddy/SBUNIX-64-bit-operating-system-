#include <stdlib.h>
#include <sys/defs.h>
#include <sys/mman.h>

#define MMAP_FILE_ARB "tmp/mmap1.bin"

//char **environ;

void *malloc(size_t size)
{
    size = size + sizeof(size);
    int *memory  = mmap(NULL,size,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE,-1,0);
    memory[0] = size; //typically this is length
    return (void *) &memory[1];
}

void free(void *p)
{ 
    int *temp = (int *) p;
    temp--;
    int size = temp[0];
    
    munmap((void *)temp, size); 

}

//malloc(sizeof(char *));
//environ[0] = '\0';
