#ifndef _phyMemMapper_h
#define _phyMemMapper_h

#include<sys/defs.h>

//pointer to keep track of available frames
uint64_t* availFrames;

//identify the row of the page in the bitmap
#define ROW(rowNum) rowNum/64
//identify the col of the page in the bitmap
#define COL(colNum) colNum%64

static void markasUsed(uint64_t frameAddr);
static void markasFree(uint64_t frameAddr);
static uint64_t isFrameUsed(uint64_t frameAddr);
static u64int getFirstFrame();

#endif
