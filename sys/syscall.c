#include<sys/syscall.h>
#include<sys/defs.h>
#include<sys/virtualMemory.h>
#include<sys/kprintf.h>
#include<sys/tarfs.h>

#include<sys/task.h>

#define MSR_EFER 0xC0000080
#define MSR_STAR 0xC0000081
#define MSR_LSTAR 0xC0000082
#define MSR_SFMASK 0xC0000084


static int mutex = 0;
int getMutex()
{
    return mutex;
}

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
    
    //kprintf("\n%d %d %d %d %d %d %d",syscallNum,paramA,paramB,paramC,paramD,paramE,paramF);
//    char* p =  (char*)paramB;
//    kprintf("%s",p);
//
    mutex =1;
    
    void* returnVal = 0;
    
    switch(syscallNum)
    {
        case 123:  returnVal =  (void*)clearScreen(); break;
        case 100:  returnVal =  (void*)exit((void*)paramA); break;
        case 199:  returnVal =  (void*)exit((void*)paramA); break;
        case 1:    returnVal =  (void*)fork(); break;
        case 99:   returnVal =  (void*)malloc(paramA); break;
        case 0:    returnVal =  (void *)readFile(paramA,(char *)paramB,paramC); break;
        case 4:    returnVal =  (void *)writeFile(paramA,(char *)paramB,paramC); break;
        case 3:    returnVal =  (void*)closeFile(paramA); break;
        case 2:    returnVal =  (void*)openFile((char *)paramA); break;
        case 9:    returnVal =  (void*)openDirectory((char *)paramA); break;
        case 10:   returnVal =  (void*)closeDir(paramA); break;
        case 11:   returnVal =  (void*)readDir(paramA,(char *)paramB,paramC); break;
        case 12:   returnVal =  (void*)getDirEntries(paramA,(char *)paramB,paramC); break;
        case 80:   returnVal =  (void*)changeDirectory((char *)paramA); break;
        case 79:   returnVal =  (void*)getCWD((char *)paramA,paramB); break;
        case 78:   returnVal =  (void*)exec((char*)paramA,(char*)paramB,(char*)paramC); break;
        case 88:   returnVal =  (void*)waitpid((void*)paramA,(uint64_t*)paramB,(void*)paramC); break;
        case 89:   returnVal =  (void*)free((void*)paramA); break;
        case 44:   returnVal =  (void*)ps(); break;
        case 420:  returnVal =  (void*)printMe((void*)paramA,(char*)paramB,(char*)paramC); break;
    }
    
    return returnVal;
    
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
