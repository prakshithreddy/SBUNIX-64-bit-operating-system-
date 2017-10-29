#ifndef _virtualMemory_h
#define _virtualMemory_h

#include<sys/defs.h>
#define get_PML4_INDEX(x)  (((x) >> 39)&0x1FF)
#define get_PDPT_INDEX(x)  (((x) >> 30)&0x1FF)
#define get_PDT_INDEX(x)  (((x) >> 21)&0x1FF)
#define get_PT_INDEX(x)  (((x) >> 12)&0x1FF)
#define PRESENT 0x1
#define WRITEABLE 0x2
#define USER 0x4
#define FRAME 0xFFFFFFFFFFFFF000

extern char kernmem


typedef struct PT
{
  uint64_t entries[512];
};

typedef struct PDT
{
  uint64_t entries[512];
};

typedef struct PDPT
{
  uint64_t entries[512];
};

typedef struct PML4
{
  uint64_t entries[512];
};

struct PML4 *pml4;
#endif
