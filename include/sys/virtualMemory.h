#ifndef _virtualMemory_h
#define _virtualMemory_h

#include<sys/defs.h>

struct PML4 {
    uint64_t entries[512];
};
struct PDPT {
    uint64_t entries[512];
};
struct PDT {
    uint64_t entries[512];
};
struct PT {
    uint64_t entries[512];
};


#endif
