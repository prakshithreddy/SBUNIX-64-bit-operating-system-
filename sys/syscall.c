#include<sys/syscall.h>

#define MSR_EFER 0xC0000080
#define MSR_STAR 0xC0000081
#define MSR_LSTAR 0xC0000082
#define MSR_SFMASK  0xC0000084

userRSP = 0;
kernelRSP = 0;

uint64_t readMSR(uint32_t msrId)
{
    uint32_t msr_lo, msr_hi;
    __asm__ __volatile__ ( "rdmsr" : "=a" (msrLow), "=d" (msrHigh) : "c" (msrId));
    return (uint64_t)(msrHigh<<32)|(uint64_t)msrLow;
}

int64_t syscallHandler(int64_t paramA,int64_t paramB,int64_t paramC,int64_t paramD,int64_t paramE,int64_t paramF,int64_t syscallNum) {
    kprintf("%d",syscallNum);
    return 0;
    
}

int64_t syscall(int64_t syscallNum,int64_t param1,int64_t param2,int64_t param3,int64_t param4,int64_t param5,int64_t param6) {
    int64_t returnValue;
    __asm__ volatile ("movq %1, %%rax; movq %2, %%rdi; movq %3, %%rsi; movq %4, %%rdx; movq %5, %%r10; movq %6, %%r8;movq %7, %%r9;syscall;movq %%rax, %0;":"=g"(returnValue):"g"(syscallNum),"g"(param1),"g"(param2),"g"(param3),"g"(param4),"g"(param5),"g"(param6):"%rax","%rdi","%rsi","%rdx","%r10","%r8","%r9","memory");//clobber list
    return returnValue;
}

void writeMSR(unint64_t value,uint32_t msrId)
{
    uint32_t msrLow;
    uint32_t msrHigh;
    msrLow = (uint32_t) value;
    msrHi = (uint32_t)(value>>32);
     __asm__ __volatile__ ("wrmsr" : : "a"(msrLow), "d"(msrHigh), "c"(msrId));
}

void _syscallEntry();

void initSyscalls()
{
    //STEP1: set the system call extension bit (SCE bit) to 1;
    uint64_t sce = readMSR(MSR_EFER);
    sce | = (0x1);
    writeMSR(sce,MSR_EFER);
    
    writeMSR((uint64_t)(0x8<<32)| (uint64_t)(0x2B <<48),MSR_STAR);
    writeMSR((uint64_t)_syscallEntry,MSR_LSTAR);
    writeMSR((1<<9),MSR_SFMASK);
    
}
