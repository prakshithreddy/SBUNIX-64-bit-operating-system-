#include <sys/defs.h>
#include <stdlib.h>
#include <sys/syscall.h>

void exit(int status){
    syscall5 ((void*)60,(void*)0,0,0,0,0);
}
