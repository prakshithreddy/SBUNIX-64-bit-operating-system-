#include<sys/phyMemMapper.h>
#include<sys/kprintf.h>
static uint64_t* availFrames;
static uint64_t KER_PHYSBASE;
static uint64_t KER_PHYSFREE;

//Using references from http://www.jamesmolloy.co.uk/tutorial_html/6.-Paging.html

uint64_t get_ker_physbase(){
  return KER_PHYSBASE;
}

uint64_t get_ker_physfree(){
  return KER_PHYSFREE;
}
void initPhys(uint64_t base,uint64_t limit)
{
    KER_PHYSBASE = base;
    KER_PHYSFREE = limit;
    
}
void markasFree(uint64_t frameNum)
{
    uint64_t row = ROW(frameNum);
    uint64_t col = COL(frameNum);
    availFrames[row] |= (0x1ULL << col);
}

void markasUsed(uint64_t frameNum)
{
    uint64_t row = ROW(frameNum);
    uint64_t col = COL(frameNum);
    availFrames[row] &= ~(0x1ULL << col);
}

uint64_t isFrameUsed(uint64_t frameNum){
    uint64_t row = ROW(frameNum);
    uint64_t col = COL(frameNum);
    return (availFrames[row] & (0x1ULL << col));
}

void allocateBitmapMem()
{
    availFrames = (uint64_t*)KER_PHYSFREE;
    
    for (int i=0;i<256;i++)
    {
        availFrames[i] = 0;
    }
    
    KER_PHYSFREE+=(256*64)/8;
    
}

void initBitmap(uint64_t start,uint64_t end)
{
    uint64_t startFrameNum = start/0x1000;
    uint64_t endFrameNum = end/0x1000;
    uint64_t temp1 = (KER_PHYSBASE/0x1000);
    uint64_t temp2 = (KER_PHYSFREE/0x1000);
    //kprintf("\ntemp1 %x  %x",temp1,temp2);
    int i=0;
    while(startFrameNum<=endFrameNum)
    {
        //if(startFrameNum>=512 && startFrameNum<=544) kprintf("\n%d %d %d",startFrameNum,temp1,temp2);
        if ((startFrameNum >= temp1) && (startFrameNum <= temp2)){
        //if ((startFrameNum <= temp2)){
          i++;
        }
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
uint64_t getFirstFrame(){
    
    for(uint64_t row=0;row<256;row++)
    {
        if (availFrames[row] != 0x0ull)
        {
            for(uint64_t col=0;col<64;col++)
            {
                uint64_t temp = 0x1ULL << col;
                if((availFrames[row]&temp))
                {
                    return row*64+col;
                }
            }
        }
    }
    return -1;
}

void* pageAllocator(){
  uint64_t frameNum=getFirstFrame();
  if (frameNum==-1){
    return NULL;
  }
  else{
    markasUsed(frameNum);
    uint64_t pageAddr = frameNum*0x1000;
    return (void*)pageAddr;
  }
}

void pageDeAllocator(void* pageAddr){
  uint64_t frameNum=((uint64_t)(pageAddr))/(0x1000ull);
  markasFree(frameNum);
}



