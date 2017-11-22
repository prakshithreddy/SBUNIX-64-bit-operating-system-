#include<sys/phyMemMapper.h>
#include<sys/kprintf.h>
static uint64_t* availFrames;
static uint64_t KER_PHYSBASE;
static uint64_t KER_PHYSFREE;
static uint64_t PHYSEND;

static uint64_t MAX_BITMAP_ROWS;
//Using references from http://www.jamesmolloy.co.uk/tutorial_html/6.-Paging.html

void set_bitmap_rows(){
  /*MAX_BITMAP_ROWS=(0xFFFFFFFFFFFFF000-0xFFFFFFFF80000000);
  MAX_BITMAP_ROWS=MAX_BITMAP_ROWS/0x1000;
  MAX_BITMAP_ROWS/=64;
  MAX_BITMAP_ROWS+=1;
  kprintf("Max rows is %d\n",MAX_BITMAP_ROWS);*/
  MAX_BITMAP_ROWS=512;
}

uint64_t get_bitmap_rows(){
  return MAX_BITMAP_ROWS;
}

uint64_t get_ker_physbase(){
  return KER_PHYSBASE;
}

uint64_t get_ker_physfree(){
  return KER_PHYSFREE;
}

uint64_t get_physend(){
  return PHYSEND;
}

void set_availFrames(uint64_t kern_base){
  uint64_t temp=(uint64_t)availFrames+kern_base;
  availFrames=(uint64_t*)temp;
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
    set_bitmap_rows();
    availFrames = (uint64_t*)KER_PHYSFREE;
    
    for (int i=0;i<MAX_BITMAP_ROWS;i++)
    {
        availFrames[i] = 0;
    }
    
    KER_PHYSFREE+=(MAX_BITMAP_ROWS*64)/8;
    
}

void initBitmap(uint64_t start,uint64_t end)
{
    uint64_t startFrameNum = start/0x1000;
    uint64_t endFrameNum = end/0x1000;
    //uint64_t temp1 = (KER_PHYSBASE/0x1000);
    uint64_t temp2 = (KER_PHYSFREE/0x1000);
    while(startFrameNum<=endFrameNum)
    {
        //if(startFrameNum>=512 && startFrameNum<=544) kprintf("\n%d %d %d",startFrameNum,temp1,temp2);
        //if ((startFrameNum >= temp1) && (startFrameNum <= temp2)){
        if ((startFrameNum <= temp2)){
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
    PHYSEND=end;
}

//returns the first free avail frame
uint64_t getFirstFrame(){
    
    for(uint64_t row=0;row<MAX_BITMAP_ROWS;row++)
    {
        if (availFrames[row] != 0x0ull)
        {
            for(uint64_t col=0;col<64;col++)
            {
                uint64_t temp = 0x1ULL << col;
                if((availFrames[row]&temp))
                {
                    //kprintf("F: %d ",row*64+col);
                    return row*64+col;
                }
            }
        }
    }
    kprintf("Frame: -1 Allocated\n");
    return -1;
}

void* pageAllocator(){
  uint64_t frameNum=getFirstFrame();
  if (frameNum==-1){
    return NULL;
  }
  else{
    markasUsed(frameNum);
    uint64_t pageAddr = frameNum*0x1000;//TODO: memset is done in kmalloc after paging, before paging needs to done after calling this function.
    return (void*)pageAddr;
  }
}

void pageDeAllocator(void* pageAddr){
  uint64_t frameNum=((uint64_t)(pageAddr))/(0x1000ull);
  markasFree(frameNum);
}

void memset(uint64_t pageaddr){
  int i=0;
  uint64_t *pointer=(uint64_t *)pageaddr;
  while(i<512){
    *pointer=0;
    pointer+=1;
    i+=1;
  }
  //kprintf("M ");
}



