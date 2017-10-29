#include<sys/phyMemMapper.h>


static void initPhys(uint64_t base,uint64_t limit)
{
    KER_PHYSBASE = base;
    KER_PHYSFREE = limit;
    
}
static void markasFree(uint64_t frameNum)
{
    uint64_t row = ROW(frameNum);
    uint64_t col = COL(frameNum);
    availFrames[row] |= (0x1UL << col);
}

static void markasUsed(uint64_t frameNum)
{
    uint64_t row = ROW(frameNum);
    uint64_t col = COL(frameNum);
    availFrames[row] &= ~(0x1UL << col);
}

static uint64_t isFrameUsed(uint64_t frameNum){
    uint64_t row = ROW(frameNum);
    uint64_t col = COL(frameNum);
    return (availFrames[row] & (0x1UL << col));
}

static void allocateBitmapMem()
{
    availFrames = (uint64_t*)KER_PHYSFREE;
    
    for (i=0;i<256;i++)
    {
        availFrames[i] = 0;
    }
    
    KER_PHYSFREE+=(256*64)/8;
    
}

static void initBitmap(uint64_t start,uint64_t end)
{
    uint64_t startFrameNum = start/0x1000;
    uint64_t endFrameNum = start/0x1000;
    while(startFrameNum<=endFrameNum)
    {
        if ((startFrameNum * PAGE_SIZE) <= KER_PHYSFREE
            && (startFrameNum * PAGE_SIZE) >= KER_PHYSBASE);
        else
        {
            markasFree(startFrameNum);
        }
        startFrameNum++;
    }
    
    //init the first one Mb to USED
    int temp = 0;
    for (uint64_t i=0;i<0x100000;i+=0x1000)
    {
        markasUsed(temp);
        temp++;
    }
}

//returns the first free avail frame
static uint64_t getFirstFrame(){
    
    for(uint64_t row=0;row<256;row++)
    {
        if (availFrames[row] != 0xFFFFFFFFFFFFFFFF)
        {
            for(uint64_t col=0;col<64;col++)
            {
                uint64_t temp = 0x1UL << col;
                if(!(availFrames[row]&temp))
                {
                    return row*64+col;
                }
            }
        }
    }
}
