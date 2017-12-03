#include<sys/syscall.h>
#include<sys/defs.h>
#include<sys/virtualMemory.h>
#include<sys/kprintf.h>

#include<sys/task.h>

#define MSR_EFER 0xC0000080
#define MSR_STAR 0xC0000081
#define MSR_LSTAR 0xC0000082
#define MSR_SFMASK 0xC0000084

uint64_t* userRSP = 0;
uint64_t* userRIP = 0;
uint64_t* kernelRSP = 0;
uint64_t userRax = 0;
uint64_t userRbx = 0;
uint64_t userRcx = 0;
uint64_t userRdx = 0;
uint64_t userRsi = 0;
uint64_t userRdi = 0;
uint64_t userRbp = 0;
uint64_t userRflags = 0;

uint64_t readMSR(uint32_t msrAddr)
{
    uint32_t msrLow, msrHigh;
    __asm__ __volatile__ ("rdmsr" :"=a"(msrLow),"=d"(msrHigh):"c"(msrAddr));
    return (uint64_t)msrHigh<<32|(uint64_t)msrLow;
}

void* syscallHandler(uint64_t paramA,uint64_t paramB,uint64_t paramC,uint64_t paramD,uint64_t paramE,uint64_t paramF,uint64_t syscallNum) {
    
    kprintf("\n%d %d %d %d %d %d %d",syscallNum,paramA,paramB,paramC,paramD,paramE,paramF);
    
    switch(syscallNum)
    {
        case 1: kprintf("Fork System Call\n"); return (void*)fork();
        case 99: kprintf("Fork System Call\n"); return malloc(paramB);
                
    }
    
    return (void*)100;
    
}

void writeMSR(uint64_t value,uint32_t msrAddr)
{
    uint32_t msrLow;
    uint32_t msrHigh;
    msrLow=(uint32_t)value;
    msrHigh=(uint32_t)(value>>32);
     __asm__ __volatile__ ("wrmsr"::"a"(msrLow),"d"(msrHigh),"c"(msrAddr));
}

void _syscallEntry();

void initSyscalls()
{
    kernelRSP = (uint64_t*)kmalloc();
    //kprintf("TSS RSP : %p ", kernelRSP);
    //STEP1: set the system call extension bit (SCE bit) to 1;
    uint64_t sce = readMSR(MSR_EFER);
    sce |= (0x1); //enable syscalls
    writeMSR(sce,MSR_EFER);
    
    writeMSR(1<<9,MSR_SFMASK);
    
    writeMSR((uint64_t)0x8<<32|(uint64_t)0x1B<<48,MSR_STAR);
    
    writeMSR((uint64_t)_syscallEntry,MSR_LSTAR);
    
    
    
}
