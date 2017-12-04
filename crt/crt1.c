#include <stdlib.h>
#include<sys/defs.h>

#define null (void*)0

void _start(void) {
  // call main() and exit() here
    uint64_t* rsp;
    
    __asm__ __volatile__("movq %%rsp,%0;":"=r"(rsp)::);
    
  main(*(rsp+1),(char**)(*(rsp+2)),(char**)(rsp+*(rsp+1)));
}

