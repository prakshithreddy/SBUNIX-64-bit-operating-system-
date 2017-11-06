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
void mapKernelMemory();
void enablePaging();
void mapPage(uint64_t v_addr, uint64_t phy_addr);
void identityMapping();
void mapVideoMemory(uint64_t vga_virtual_address);

extern char kernmem, physbase;

struct PT
{
  uint64_t entries[512];
};

struct PDT
{
  uint64_t entries[512];
};

struct PDPT
{
  uint64_t entries[512];
};

struct PML4
{
  uint64_t entries[512];
};
#endif
