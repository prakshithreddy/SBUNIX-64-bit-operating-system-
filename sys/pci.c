#include<sys/pci.h>
#include<sys/defs.h>
#include<sys/ahci.h>
#include<sys/virtualMemory.h>
#include<sys/phyMemMapper.h>

uint16_t dataPort = 0xCFC;
uint16_t commandPort = 0xCF8;
static volatile hba_mem_t *abar;
static volatile hba_mem_t *virtual_abar;
//static volatile hba_mem_t *abar = (hba_mem_t *)AHCI_BASE;

int32_t inb_32(uint16_t port)
{
    uint32_t val;
    __asm__ __volatile__("inl %1,%0;":"=a" (val): "Nd" (port));
    return val;
}


void outb_32(uint32_t data,uint16_t port)
{
    __asm__ __volatile__("outl %0,%1;": :"a" (data),"Nd" (port));
}

uint32_t readPIC(uint16_t bus,uint16_t device,uint16_t function,uint32_t offset)
{
    uint32_t command = (uint32_t)(((0x01<<31)|(bus&0xFF)<<16|(device&0x1F)<<11|(function&0x07)<<8|(offset&0xFC))|0x80000000);
    outb_32(command,commandPort);
    uint32_t result = inb_32(dataPort);
    //return result >> (8*(offset&2));
    return result;
}

void readPIC2 (uint16_t bus,uint16_t device,uint16_t function,uint32_t offset,uint32_t abar)
{
    //uint32_t command = (uint32_t)((0x00<<31)|(bus&0xFF)<<16|(device&0x1F)<<11|(function&0x07)<<8|(offset&0xFC));
    //outb_32(command,commandPort);
    outb_32(abar,dataPort);
    //inb_32(dataPort);
    //uint32_t result = inb_32(dataPort);
    //return result;
}

int ifMultiFunction(uint16_t bus,uint16_t device)
{
    int multifunction= readPIC(bus,device,0,0x0C) & (0x00800000);
    if(multifunction == 0x00800000){
      return 1;
    }
    return 0;
}

void writePIC(uint16_t bus,uint16_t device,uint16_t function,uint32_t offset,uint32_t value)
{
    uint32_t command = (uint32_t)((0x01<<31)|(bus&0xFF)<<16|(device&0x1F)<<11|(function&0x07)<<8|(offset&0xFC));
    outb_32(command,commandPort);
    outb_32(value,dataPort);
}

void probeAHCI(){
  port_probe(virtual_abar);
}

void printALLDrivers()
{   kprintf("Walking through the PCI Configuration Space...\n"); 
    for (int bus=0;bus<256;bus++)
    {
        for(int device=0;device<32;device++)
        {
            //int multiFunction = ifMultiFunction(bus,device)>0?8:1;
            for(int function =0;function<8;function++)
            {
                uint32_t vendorID = readPIC(bus,device,function,0x00);
                uint32_t class_ID = readPIC(bus,device,function,0x08);
                //uint32_t header = readPIC(bus,device,function,0x0C);
                
                if((vendorID&0xFFFF)==0xFFFF) continue;
                //kprintf("%p\n",(header));
                if(((class_ID&0xFFFF0000)>>16)==0x0106){
                  kprintf("AHCI Device Found...\n");
                  kprintf("    Bus %d Device %d Function %d \n",bus,device,function);
                  kprintf("    Vendor ID: %x\n",(vendorID));
                  kprintf("    Class ID: %x\n",(class_ID));
                  uint64_t bar_5=readPIC(bus,device,function,36);
                  kprintf("Bar5 Found :D | Physical Bar5: %p \n",bar_5);
                  abar=(volatile hba_mem_t *)bar_5;
                  virtual_abar = (volatile hba_mem_t *)(mapAHCI((uint64_t)bar_5));
                }
            }
        }
    }
    kprintf("...\n");
}
