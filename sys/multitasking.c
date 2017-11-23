#include<sys/task.h>
#include<sys/kprintf.h>
#include<sys/phyMemMapper.h>
#include<sys/virtualMemory.h>
#include<sys/gdt.h>
#include<sys/syscall.h>

static Task *runningThread;
static Task mainThread;
static Task otherThread;

Task *userThread1;
Task *userThread2;

uint64_t* currentRSP = 0; // keep a current RSP
uint64_t currentRAX;

void createThread(Task *kthread, void(*function)(), uint64_t rflags, uint64_t *pml4){
    kthread->regs.rax=0;
    kthread->regs.rbx=0;
    kthread->regs.rcx=0;
    kthread->regs.rdx=0;
    kthread->regs.rsi=0;
    kthread->regs.count=0;
    kthread->regs.add=0;
    kthread->regs.rdi=0;
    kthread->regs.rflags=rflags;
    kthread->regs.rip=(uint64_t)function;
    kthread->regs.cr3=(uint64_t)pml4;
    kthread->regs.userRsp=(uint64_t)kmalloc()+0x1000;
    kthread->regs.rbp=kthread->regs.userRsp; //doing this because rbp is base pointer of stack.
    kthread->next=0;
}

static void multitaskMain() {
    //while(1);
    kprintf("Creating Kernel Threads..");
    yield();
    //  i+=1;
    //  kprintf("%d",i);
    //  yield();
}

void initMultiTasking() {
    __asm__ __volatile__("movq %%cr3, %%rax; movq %%rax, %0;":"=m"(mainThread.regs.cr3)::"%rax");
    __asm__ __volatile__("pushfq; movq (%%rsp), %%rax; movq %%rax, %0; popfq;":"=m"(mainThread.regs.rflags)::"%rax");
    createThread(&otherThread, multitaskMain, mainThread.regs.rflags, (uint64_t*)mainThread.regs.cr3);
    //mainThread.regs.rip=(uint64_t)start;
    mainThread.next = &otherThread;
    otherThread.next = &mainThread;
    runningThread = &mainThread;
    yield();
    kprintf("Multitasking Success\n");
    //yield();
}

void yield() {
    Task *last = runningThread;
    runningThread = runningThread->next;
    _switchThread_(&last->regs, &runningThread->regs);
}

void _switchThread_(Registers *from, Registers *to);


static void userProcess1() {
    //static int i=0;
    //uint64_t temp;
    kprintf("In User Space1........");
//    __asm__ __volatile__("movq %%rsp, %%rax; movq %%rax, %0;":"=m"(temp)::"%rax");
//    kprintf("%p ", temp);
    //__asm__ __volatile__ ("int $0x10":::);
    uint64_t retVal = syscall(10,1,2,3,4,5,6);
    kprintf("%d",retVal);
////    int i=1;
////    int j=0;
//     j=i/j;
//    kprintf("%d",j);
    while(1);
    //yield();
    //  i+=1;
    //  kprintf("%d",i);
    //  yield();
}

static void userProcess2() {
    //static int i=0;
    kprintf("In User Space2........");
    uint64_t retVal = syscall(10,1,2,3,4,5,6);
    kprintf("%d",retVal);
//    //__asm__ __volatile__ ("int $0x10":::);
//    uint64_t retVal = syscall(10,1,2,3,4,5,6);
//    kprintf("%d",retVal);
    //    int i=1;
    //    int j=0;
    //     j=i/j;
    //    kprintf("%d",j);
    while(1);
    //yield();
    //  i+=1;
    //  kprintf("%d",i);
    //  yield();
}

void _prepareInitialKernelStack(Registers* current);

void createUserProcess(Task *kthread, void(*function)(), uint64_t rflags){
    kthread->regs.rax=0;
    kthread->regs.rbx=0;
    kthread->regs.rcx=0;
    kthread->regs.rdx=0;
    kthread->regs.rsi=0;
    kthread->regs.rdi=0;
    kthread->regs.rflags=rflags;
    kthread->regs.rip=(uint64_t)function;
    kthread->regs.cr3=(uint64_t)getNewPML4ForUser();
    kthread->regs.userRsp=(uint64_t)kmalloc()+0x1000;   // creating a stack for the user process
    kthread->regs.kernelRsp=(uint64_t)kmalloc()+0x1000; // creating a stack for the kernel code of the user process
    kthread->regs.rbp=kthread->regs.userRsp; //doing this because rbp is base pointer of stack.
    kthread->regs.count=0;
    kthread->regs.add=0;
    kthread->next=0;
    _prepareInitialKernelStack(&kthread->regs);
}

void _switchToRingThree(Registers *from, Registers *to);
void _moveToNextProcess(Registers* prev,Registers* next);

void runNextTask()
{
    //update the currentRunning Task
    Task *prev = runningThread;
    runningThread = runningThread->next;
    if (runningThread->regs.count==0)
    {    runningThread->regs.add=40;
        runningThread->regs.count+=1;
    }
    else
    {
        runningThread->regs.add=0;
        
    }
    int64_t tssAddr = runningThread->regs.kernelRsp;
    set_tss_rsp((void*)(tssAddr));
    _moveToNextProcess(&prev->regs, &runningThread->regs);
}

void switchToUserMode()
{
    Task *last = runningThread;
    runningThread = runningThread->next;
    if (runningThread->regs.count==0)
    {
        runningThread->regs.add=40;
        runningThread->regs.count+=1;
    }
    
    else
    {
        runningThread->regs.add=0;
    
    }
    uint64_t tssAddr = runningThread->regs.kernelRsp;
    set_tss_rsp((void*)(tssAddr));
    _switchToRingThree(&last->regs, &runningThread->regs);
    
}
void initUserProcess()
{
    
    
    userThread1 = (Task*)kmalloc();
    userThread2 = (Task*)kmalloc();
    
    createUserProcess(userThread1,userProcess1,mainThread.regs.rflags);
    createUserProcess(userThread2,userProcess2,mainThread.regs.rflags);
    
    mainThread.next = userThread1;
    userThread1->next = userThread2;
//    userThread2->next = &mainThread;
    userThread2->next = userThread1; //temp, just to see what happens :P
    
    switchToUserMode();
    
}
