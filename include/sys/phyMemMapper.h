#ifndef _phyMemMapper_h
#define _phyMemMapper_h

#include<sys/defs.h>

//pointer to keep track of available frames
uint64_t* availFrames;
#define PAGE_SIZE 4096

static uint64_t KER_PHYSBASE;
static uint64_t KER_PHYSFREE;

static void initPhys(uint64_t base,uint64_t limit);

//identify the row of the page in the bitmap
#define ROW(rowNum) rowNum/64
//identify the col of the page in the bitmap
#define COL(colNum) colNum%64

static void markasUsed(uint64_t frameNum);
static void markasFree(uint64_t frameNum);
static uint64_t isFrameUsed(uint64_t frameNum);
static u64int getFirstFrame();
static void initBitmap(uint64_t start,uint64_t end);
static void allocateBitmapMem();

#endif
