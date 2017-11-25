#include <stdlib.h>
#include<sys/defs.h>

void _start(void) {
  // call main() and exit() here
    uint64_t* rsp;
    __asm__ __volatile__("%%rsp,%0;":"=r"(rsp)::);
  main(*(rsp+1));
}

