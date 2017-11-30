#ifndef _phyMemMapper_h
#define _phyMemMapper_h

#include<sys/defs.h>

//pointer to keep track of available frames
#define PAGE_SIZE 4096

void initPhys(uint64_t base,uint64_t limit);

//identify the row of the page in the bitmap
#define ROW(rowNum) rowNum/64
//identify the col of the page in the bitmap
#define COL(colNum) colNum%64

void markasUsed(uint64_t frameNum);
void markasFree(uint64_t frameNum);
uint64_t isFrameUsed(uint64_t frameNum);
uint64_t getFirstFrame();
void initBitmap(uint64_t start,uint64_t end);
void allocateBitmapMem();
void* pageAllocator();
void pageDeAllocator(void* pageAddr);
uint64_t get_ker_physbase();
uint64_t get_ker_physfree();
uint64_t get_physend();
void memset(uint64_t pageaddr);
void set_availFrames(uint64_t kern_base);
void set_bitmap_rows();
uint64_t get_bitmap_rows();
uint64_t get_AHCI_PHYS();
void allocateAHCI();

void invlpg(uint64_t addr);

#define PHYSSTART 0x100000;

#endif
