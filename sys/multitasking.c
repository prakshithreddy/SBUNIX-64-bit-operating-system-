#include<sys/task.h>
#include<sys/kprintf.h>
#include<sys/phyMemMapper.h>
#include<sys/virtualMemory.h>

static kernelThread *runningThread;
static kernelThread mainThread;
static kernelThread otherThread;

extern

void createThread(kernelThread *kthread, void(*function)(), uint64_t rflags, uint64_t *pml4){
  kthread->regs.rax=0;
  kthread->regs.rbx=0;
  kthread->regs.rcx=0;
  kthread->regs.rdx=0;
  kthread->regs.rsi=0;
  kthread->regs.rdi=0;
  kthread->regs.rflags=rflags;
  kthread->regs.rip=(uint64_t)function;
  kthread->regs.cr3=(uint64_t)pml4;
  kthread->regs.rsp=(uint64_t)kmalloc()+0x1000;
  kthread->regs.rbp=kthread->regs.rsp; //doing this because rbp is base pointer of stack.
  kthread->next=0;
}

static void multitaskMain() {
  //static int i=0;
  kprintf("Enabling multithreaded Kernel.....");
  //while(1);
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
  kprintf("Success\n");
  //yield();
}

void yield() {
  kernelThread *last = runningThread;
  runningThread = runningThread->next;
  _switchThread_(&last->regs, &runningThread->regs);
}

void _switchThread_(Registers *from, Registers *to);


static void userProcess() {
    //static int i=0;
    kprintf("In User Space........");
    //__asm__ __volatile__ ("int $0x10":::);
    while(1);
    //yield();
    //  i+=1;
    //  kprintf("%d",i);
    //  yield();
}

void createUserProcess(kernelThread *kthread, void(*function)(), uint64_t rflags){
    kthread->regs.rax=0;
    kthread->regs.rbx=0;
    kthread->regs.rcx=0;
    kthread->regs.rdx=0;
    kthread->regs.rsi=0;
    kthread->regs.rdi=0;
    kthread->regs.rflags=rflags;
    kthread->regs.rip=(uint64_t)function;
    kthread->regs.cr3=(uint64_t)getNewPML4ForUser();
    kthread->regs.rsp=(uint64_t)kmalloc()+0x1000;
    kthread->regs.rbp=kthread->regs.rsp; //doing this because rbp is base pointer of stack.
    kthread->next=0;
}

void _switchToRingThree(Registers *from, Registers *to);

void switchToUserMode()
{
    kernelThread *last = runningThread;
    runningThread = runningThread->next;
    _switchToRingThree(&last->regs, &runningThread->regs);
    
}
void initUserProcess()
{
    
    kernelThread *userThread = (kernelThread*)kmalloc();
    createUserProcess(userThread,userProcess,mainThread.regs.rflags);
    mainThread.next = userThread;
    userThread->next = &mainThread;
    switchToUserMode();
    
}
