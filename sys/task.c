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
static int pageNum = 0;

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

Task* getRunningThread()
{
    return runningThread;
}

uint64_t getNextPageNum()
{
    pageNum+=1;
    return pageNum;
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

void createNewTask(Task *task,uint64_t function, uint64_t rflags,uint64_t cr3){
    
    task->pid_t = pidCount+1;
    pidCount+=1; //next process takes the next id
    task->ppid_t = 0; //setting the Pid of the parent for COW
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

int markPageAsRW(uint64_t v_addr,uint64_t cr3,int rw){
    
    struct PDPT *pdpt;//TODO: This function is used only after enabling paging.
    struct PDT *pdt;
    struct PT *pt;
    struct PDPT *v_pdpt;
    struct PDT *v_pdt;
    struct PT *v_pt;
    
    struct PML4 *curr_pml4=(struct PML4*)(cr3);
    
    uint64_t pml_entry = curr_pml4->entries[get_PML4_INDEX((uint64_t)v_addr)];
    
    if(pml_entry&PRESENT){
        pdpt = (struct PDPT*)(pml_entry&FRAME);
        v_pdpt = (struct PDPT*)((uint64_t)pdpt+get_kernbase());
        uint64_t pdpt_entry = v_pdpt->entries[get_PDPT_INDEX((uint64_t)v_addr)];
        if(pdpt_entry&PRESENT){
            pdt = (struct PDT*)(pdpt_entry&FRAME);
            v_pdt = (struct PDT*)((uint64_t)pdt+get_kernbase());
            uint64_t pdt_entry = v_pdt->entries[get_PDT_INDEX((uint64_t)v_addr)];
            if(pdt_entry&PRESENT){
                pt = (struct PT*)(pdt_entry&FRAME);
                v_pt = (struct PT*)((uint64_t)pt+get_kernbase());
                if(rw)
                {
                    //setPage as read only
                    v_pt->entries[get_PT_INDEX((uint64_t)v_addr)] = v_pt->entries[get_PT_INDEX((uint64_t)v_addr)]&MAKERDONLY;
                }
                else
                {
                    //setPage as writable
                    v_pt->entries[get_PT_INDEX((uint64_t)v_addr)] = v_pt->entries[get_PT_INDEX((uint64_t)v_addr)]|WRITEABLE;
                    
                }
                return 0;
                
            }
            
        }
    }
    return -1;
    
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


uint64_t getPhysicalPageAddr(uint64_t v_addr,uint64_t cr3){
    
    struct PDPT *pdpt;//TODO: This function is used only after enabling paging.
    struct PDT *pdt;
    struct PT *pt;
    struct PDPT *v_pdpt;
    struct PDT *v_pdt;
    struct PT *v_pt;
    
    struct PML4 *curr_pml4=(struct PML4*)(cr3+get_kernbase());
    
    uint64_t pml_entry = curr_pml4->entries[get_PML4_INDEX((uint64_t)v_addr)];
    
    if(pml_entry&PRESENT){
        pdpt = (struct PDPT*)(pml_entry&FRAME);
        v_pdpt = (struct PDPT*)((uint64_t)pdpt+get_kernbase());
        uint64_t pdpt_entry = v_pdpt->entries[get_PDPT_INDEX((uint64_t)v_addr)];
        if(pdpt_entry&PRESENT){
            pdt = (struct PDT*)(pdpt_entry&FRAME);
            v_pdt = (struct PDT*)((uint64_t)pdt+get_kernbase());
            uint64_t pdt_entry = v_pdt->entries[get_PDT_INDEX((uint64_t)v_addr)];
            if(pdt_entry&PRESENT){
                pt = (struct PT*)(pdt_entry&FRAME);
                v_pt = (struct PT*)((uint64_t)pt+get_kernbase());
                return v_pt->entries[get_PT_INDEX((uint64_t)v_addr)];
            }
            
        }
    }
    return -1;
    
}


void copyParentCr3Entries(Task* task)
{
    VMA* vma = runningThread->memMap.mmap;
    uint64_t temp = get_stack_top();
    uint64_t cur_stack_start = (runningThread->regs.userRsp)&FRAME;
    while(vma!=NULL)
    {
        uint64_t start = vma->v_start;
        while(start<vma->v_end)
        {   
            if(vma->v_end != temp){
                mapPageForUser(start&FRAME,getPhysicalPageAddr(start,runningThread->regs.cr3),task->regs.cr3+get_kernbase());
                start = start + 0x1000;
            }
            else{
                uint64_t phy_child_stack = (uint64_t)kmalloc();
                phy_child_stack-=get_kernbase();
                mapPageForUser(start&FRAME,phy_child_stack,task->regs.cr3+get_kernbase());
                memcpy((void *)start&FRAME,(void *)(phy_child_stack+get_kernbase()),4096);
                start = start + 0x1000;
                cur_stack_start+=0x1000;
            }
        }
        vma=vma->next;
    }
}



void copyStacktoChild(Task* task)
{
    uint64_t cur_stack_start = (runningThread->regs.userRsp)&FRAME;
    mapPageForUser(0xFFFFFF7F00000000-0x1000,(task->regs.userRsp-0x1000)-get_kernbase(),task->regs.cr3+get_kernbase());
    memcpy((void *)cur_stack_start,(void *)(0xFFFFFF7F00000000-0x1000),4096);
}
//fork process code starts here.

void copyVMA(Task* task,VMA* vma)
{
    task->memMap.mmap = NULL;
    VMA* oldHead = vma;
    while(oldHead!=NULL)
    {
        VMA* newVma = (VMA*)kmalloc();
        if(newVma!=NULL)
        {
            newVma->pageNumber = oldHead->pageNumber;
            newVma->v_mm = &task->memMap;
            task->memMap.count+=1;
            newVma->grows_down=oldHead->grows_down;
            newVma->v_start=oldHead->v_start;
            newVma->v_end=oldHead->v_end;
            newVma->mmsz=oldHead->mmsz;
            newVma->v_flags = oldHead->v_flags;
            newVma->v_offset = oldHead->v_offset;
            newVma->v_file = oldHead->v_file;
            
            VMA* temp = task->memMap.mmap;
            if(temp==NULL)
            {
                task->memMap.mmap = newVma;
                newVma->next=NULL;
            }
            else
            {
                while(temp->next!=NULL)temp=temp->next;
                temp->next=newVma;
                newVma->next=NULL;
                
            }
        }
        
        oldHead=oldHead->next;
    }
}

void createChildTask(Task *task){
    
    task->pid_t = pidCount+1;
    pidCount+=1; //next process takes the next id
    task->ppid_t = runningThread->pid_t; //setting the Pid of the parent for COW
    
    task->regs.rax=0;
    task->regs.rbx=userRbx;
    task->regs.rcx=userRcx;
    task->regs.rdx=userRdx;
    task->regs.rsi=userRsi;
    task->regs.rdi=userRdi;
    task->regs.rflags=userRflags;
    task->regs.rip=(uint64_t)userRIP;
    task->regs.cr3=(uint64_t)getNewPML4ForUser();
    
    copyParentCr3Entries(task);
    //copyStacktoChild(task);
    task->regs.userRsp=(uint64_t)userRSP;
    task->regs.kernelRsp=(uint64_t)kmalloc()+0x1000;
    task->regs.count=0;
    task->regs.add=0;
    //task->next=0;
   // task->memMap.mmap=runningThread->memMap.mmap;
    copyVMA(task,runningThread->memMap.mmap);
    _prepareInitialKernelStack(&task->regs);
}


int isPartofCurrentVma(uint64_t addr)
{
    VMA* vma = runningThread->memMap.mmap;
    while(vma!=NULL)
    {
        if((vma->v_start&FRAME)==addr) return 1;
        vma=vma->next;
    }
    
    return 0;
}


void addChildToQueue(Task* task)
{
    task->next = runningThread->next;
    runningThread->next = task;

}

void makeParentCr3asReadOnly()
{
    VMA* vma = runningThread->memMap.mmap;
    uint64_t temp = get_stack_top();
    while(vma!=NULL)
    {
        uint64_t start = vma->v_start;
        while(start<vma->v_end)
        {
            if(vma->v_end != temp){
                markPageAsRW(start&FRAME,runningThread->regs.cr3+get_kernbase(),1);
            }
            else{
                //this is stack leave it
            }
            start+=0x1000;
        }
        vma=vma->next;
    }
}

uint64_t fork()
{
    Task* child = (Task*)kmalloc();
    createChildTask(child);
    
    addChildToQueue(child);
    makeParentCr3asReadOnly();
    return child->pid_t;
}



void makePageCopiesForChilden(uint64_t pNum,Task* task)
{
    Task* tempTask = task->next;
    while(tempTask!=task)
    {
        VMA* tempVMA = tempTask->memMap.mmap;
        while(tempVMA!=NULL)
        {
            if(tempVMA->pageNumber == pNum)
            {
                //copy this page
                uint64_t virAddr = (tempVMA->v_start&FRAME);
                uint64_t newPage = (uint64_t)kmalloc();
                newPage-=get_kernbase();
                mapPageForUser(virAddr,newPage,tempTask->regs.cr3+get_kernbase());
                memcpy((void *)virAddr,(void *)(newPage+get_kernbase()),4096);
                tempVMA->pageNumber = getNextPageNum();
            }
            tempVMA=tempVMA->next;
        }
        
        tempTask=tempTask->next;
    }
    
}

uint64_t getPageNumFromAddr(uint64_t addr)
{
    VMA* temp = runningThread->memMap.mmap;
    while(temp!=NULL)
    {
        if((temp->v_start&FRAME) == addr) return temp->pageNumber;
        
        temp=temp->next;
    }
    return -1;
}

uint64_t malloc(uint64_t size)
{
    VMA* temp = runningThread->memMap.mmap;
    VMA* newVma = (VMA*)kmalloc();
    
    if(temp==NULL)
    {
        newVma->pageNumber = getNextPageNum();
        newVma->v_mm = &runningThread->memMap;
        newVma->v_start = 0x0;
        newVma->v_end = newVma->v_start+size;
        newVma->mmsz = size;
        newVma->v_flags = 0;
        newVma->grows_down = 0;
        newVma->v_file = 0;
        newVma->next=NULL;
        newVma->v_offset=0;
        
        runningThread->memMap.mmap=newVma;
        
    }
    else
    {
        uint64_t end=0;
        while(temp->next!=NULL)
        {
            if(!temp->next->grows_down && !temp->next->v_file)
                end = temp->next->v_end;
            temp = temp->next;
        }
        
        newVma->pageNumber = getNextPageNum();
        newVma->v_mm = &runningThread->memMap;
        newVma->v_start = end;
        newVma->v_end = newVma->v_start+size;
        newVma->mmsz = size;
        newVma->v_flags = 0;
        newVma->grows_down = 0;
        newVma->v_file = 0;
        newVma->next=NULL;
        newVma->v_offset=0;
        
        temp->next = newVma->next;
        temp->next = newVma;
    }
    return newVma->v_start;
    
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
    createNewTask(userThread2,hello_entrypoint,mainThread.regs.rflags,U2_cr3);
    
    mainThread.next = userThread2;
    //mainThread.next = userThread3;
    //userThread3->next = userThread1;
    //vir_userThread1->next = vir_userThread2;
//    userThread2->next = &mainThread;
    userThread2->next = userThread2; //temp, just to see what happens :P
    
    enableIntr();
    switchToUserMode();
    
}
