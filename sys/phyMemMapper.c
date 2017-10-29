#include<sys/phyMemMapper.h>


static void markasUsed(uint64_t frameAddr)
{
    uint64_t frameNum = frameAddr/0x1000;
    uint64_t row = ROW(frameNum);
    uint64_t col = COL(frameNum);
    availFrames[row] |= (0x1UL << col);
}

static void markasFree(uint64_t frameAddr)
{
    uint64_t frameNum = frameAddr/0x1000;
    uint64_t row = ROW(frameNum);
    uint64_t col = COL(frameNum);
    availFrames[row] &= ~(0x1UL << col);
}

static uint64_t isFrameUsed(uint64_t frameAddr){
    uint64_t frameNum = frameAddr/0x1000;
    uint64_t row = ROW(frameNum);
    uint64_t col = COL(frameNum);
    return (availFrames[row] & (0x1UL << col));
}

//returns the first free avail frame
static u64int getFirstFrame(){
    
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
