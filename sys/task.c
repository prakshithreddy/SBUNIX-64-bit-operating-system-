#include<sys/task.h>
#include<sys/kprintf.h>
#include<sys/phyMemMapper.h>
#include<sys/virtualMemory.h>
#include<sys/gdt.h>
#include<sys/idt.h>
#include<sys/pic.h>
#include<sys/syscall.h>
#include<sys/tarfs.h>

static Task *runningThread;
static Task mainThread;
static Task otherThread;

static int pidCount = 0;

Task *userThread1;
Task *userThread2;
//Task *userThread3;

//Task *vir_userThread1;
//Task *vir_userThread2;
//Task *vir_userThread3;

uint64_t* currentRSP = 0; // keep a current RSP
uint64_t currentRAX;

uint64_t* getRunKRsp()
{
    return (uint64_t*)runningThread->regs.kernelRsp;
}

uint64_t getRunCr3()
{
    return runningThread->regs.cr3;
}

void createThread(Task *task, void(*function)(), uint64_t rflags, uint64_t *pml4){
    task->regs.rax=0;
    task->regs.rbx=0;
    task->regs.rcx=0;
    task->regs.rdx=0;
    task->regs.rsi=0;
    task->regs.count=0;
    task->regs.add=0;
    task->regs.rdi=0;
    task->regs.rflags=rflags;
    task->regs.rip=(uint64_t)function;
    task->regs.cr3=(uint64_t)pml4;
    task->regs.userRsp=(uint64_t)kmalloc()+0x1000;
    task->regs.rbp=task->regs.userRsp; //doing this because rbp is base pointer of stack.
    task->next=0;
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


/*static void userProcess1() {
    //static int i=0;
    //uint64_t temp;
    //kprintf("In User Space1........");
//    __asm__ __volatile__("movq %%rsp, %%rax; movq %%rax, %0;":"=m"(temp)::"%rax");
//    kprintf("%p ", temp);
    //__asm__ __volatile__ ("int $0x10":::);
    uint64_t retVal = syscall(10,1,2,3,4,5,6);
    kprintf("%d",retVal);
////    int i=1;
////    int j=0;
//     j=i/j;
//    kprintf("%d",j);
    while(1);//kprintf("1 ");
    //yield();
    //  i+=1;
    //  kprintf("%d",i);
    //  yield();
}*/

/*static void userProcess2() {
    //static int i=0;
    //kprintf("In User Space2........");
    uint64_t retVal = syscall(11,1,2,3,4,5,6);
    kprintf("%d",retVal);
//    //__asm__ __volatile__ ("int $0x10":::);
//    uint64_t retVal = syscall(10,1,2,3,4,5,6);
//    kprintf("%d",retVal);
    //    int i=1;
    //    int j=0;
    //     j=i/j;
    //    kprintf("%d",j);
    
    while(1);//kprintf("2 ");
    //yield();
    //  i+=1;
    //  kprintf("%d",i);
    //  yield();
}*/

void _prepareInitialKernelStack(Registers* current);

void _pushVal(uint64_t userRsp,int val);

void pushSomeArgsToUser(uint64_t userRsp)
{
    _pushVal(userRsp,123);
}

void createNewTask(Task *task,uint64_t function, uint64_t rflags,uint64_t cr3,int parentPid){
    
    task->pid_t = pidCount+1;
    task->ppid_t = parentPid; //setting the Pid of the parent for COW
    task->regs.rax=0;
    task->regs.rbx=0;
    task->regs.rcx=0;
    task->regs.rdx=0;
    task->regs.rsi=0;
    task->regs.rdi=0;
    task->regs.rflags=rflags;
    task->regs.rip=(uint64_t)function;
    task->regs.cr3=cr3;
    task->regs.userRsp=(uint64_t)stackForUser(task)+0x1000;   // creating a stack for the user process
    pushSomeArgsToUser(task->regs.userRsp);
    task->regs.userRsp-=8;
    task->regs.kernelRsp=(uint64_t)kmalloc()+0x1000; // creating a stack for the kernel code of the user process
    task->regs.rbp=task->regs.userRsp; //doing this because rbp is base pointer of stack.
    task->regs.count=0;
    task->regs.add=0;
    task->next=0;
    //task->memMap.mmap=((void *)0);
    _prepareInitialKernelStack(&task->regs);
}

void _switchToRingThree(Registers *from, Registers *to);
void _moveToNextProcess(Registers* prev,Registers* next);

void runNextTask()
{
    runningThread->regs.kernelRsp = switchRsp;
    runningThread->regs.rax=switchRax;
    runningThread->regs.rbx=switchRbx;
    runningThread->regs.rcx=switchRcx;
    runningThread->regs.rdx=switchRdx;
    runningThread->regs.rsi=switchRsi;
    runningThread->regs.rdi=switchRdi;
    runningThread->regs.rbp=switchRbp;
    //update the currentRunning Task 
    Task *prev = runningThread;
    runningThread = runningThread->next;
    uint64_t tssAddr=0;
    if (runningThread->regs.count==0)
    {    runningThread->regs.add=40;
        runningThread->regs.count+=1;
        tssAddr = runningThread->regs.kernelRsp;
    }
    else
    {
        runningThread->regs.add=0;
        tssAddr = runningThread->regs.kernelRsp +40;
    }
    //uint64_t tssAddr = runningThread->regs.kernelRsp +40; NOTE: Moved into if else block, to make sure it does cross above the allocated page.
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

Task* createCOWTask(Task* parent)
{
    Task* task = (Task*)kmalloc();
    
    task->pid_t = pidCount+1;
    task->ppid_t = parent->pid_t; //setting the Pid of the parent for COW
    
    __asm__ __volatile__ ("movq %%rax,%0; movq %%rbx,%1; movq %%rcx,%3; movq %%rdx,%4; movq %%rsi,%5; movq %%rdi, %6; movq %%rbp,%7; pushfq; pop %8": "=a" (task->regs.rax),"=a" (task->regs.rbx),"=a" (task->regs.rcx),"=a" (task->regs.rdx),"=a" (task->regs.rsi),"=a" (task->regs.rdi),"=a" (task->regs.rax),"=a" (task->regs.rflags)::);
   
    task->regs.rax =0; //why 0 ; because syscall return to child must be zero
    task->regs.rip=(uint64_t)userRIP;
    task->regs.cr3=parent->regs.cr3;
    task->regs.userRsp= (uint64_t)userRSP;  // creating a stack for the user process
    task->regs.kernelRsp=(uint64_t)kmalloc()+0x1000; // the kernel stack should be diff for interrupts
    task->regs.rbp=parent->regs.userRsp; //doing this because rbp is base pointer of stack.
    task->regs.count=0;
    task->regs.add=0;
    task->memMap.mmap=((void *)0);
    _prepareInitialKernelStack(&task->regs);
    addCurrentTasktoRunQueue(task);
                          
  return task;
}

void addCurrentTasktoRunQueue(Task* task)
{
    //insert this task at the end / / /
    Task* nextToCurrentTask = runningThread;
    
    while(nextToCurrentTask->next!=runningThread)
    {
        nextToCurrentTask=nextToCurrentTask->next;
    }
    
    nextToCurrentTask->next = task;
    task->next = nextToCurrentTask->next;

}

                          
void markPagesAsReadOnly(uint64_t cr3)
{
    VMA *temp = runningThread->memMap.mmap;
    
    while(temp!=NULL)
    {
        uint64_t start = temp->v_start;
        while(start<temp->v_end)
        {
            struct PML4 *curr_pml4=(struct PML4*)cr3;//temp that is passed is already virtual..
            
            uint64_t pml_entry = curr_pml4->entries[get_PML4_INDEX((uint64_t)start)];
            pml_entry&=MAKERDONLY;
            curr_pml4->entries[get_PML4_INDEX((uint64_t)start)]=pml_entry;
        
            start+=0x1000;
        }
        temp=temp->next;
    }
    
    
}
                          

int fork()
{
    Task* child = createCOWTask(runningThread);
    //mark all the entries in the CR3 as readable
    markPagesAsReadOnly(runningThread->regs.cr3+get_kernbase());
    return child->pid_t;
    
}
void initUserProcess()
{
    
    //uint64_t U1_cr3 = (uint64_t)getNewPML4ForUser();
    uint64_t U2_cr3 = (uint64_t)getNewPML4ForUser();
    
    //userThread1 = (Task*)kmallocForUser(U1_cr3);
    //userThread2 = (Task*)kmallocForUser(U2_cr3);
    userThread2 = (Task*)kmalloc();
    //vir_userThread1 = (Task*)((uint64_t)userThread1+get_kernbase());
    //vir_userThread2 = (Task*)((uint64_t)userThread2+get_kernbase());
    
    //createNewTask((Task*)((uint64_t)userThread1+get_kernbase()),(uint64_t)userProcess1,mainThread.regs.rflags,U1_cr3);
    uint64_t hello_entrypoint = (loadFile("bin/sbush",(U2_cr3+get_kernbase()),userThread2));
    kprintf("Entry Point: %p\n",hello_entrypoint);
    createNewTask(userThread2,hello_entrypoint,mainThread.regs.rflags,U2_cr3,-1);
    
    mainThread.next = userThread2;
    //mainThread.next = userThread3;
    //userThread3->next = userThread1;
    //vir_userThread1->next = vir_userThread2;
//    userThread2->next = &mainThread;
    userThread2->next = userThread2; //temp, just to see what happens :P
    
    enableIntr();
    switchToUserMode();
    
}
