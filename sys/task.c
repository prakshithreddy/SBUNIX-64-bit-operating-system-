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

Task* getRunTask(){
    return runningThread;
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
    //*****************
    runningThread->fd_pointers[0]=(uint64_t)kmalloc();
    runningThread->fd_count=5;
    //*******************
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

void _pushVal(uint64_t userRsp,void* val);

void pushSomeArgsToUser(uint64_t userRsp,void* val,uint64_t cr3)
{
    uint64_t tempCr3;
    __asm__ __volatile__("mov %%cr3,%0":"=b"((uint64_t)tempCr3)::);
    __asm__ __volatile__("mov %0,%%cr3":: "b"((uint64_t)cr3):);
    invlpg(userRsp);
    _pushVal(userRsp,val);
    __asm__ __volatile__("mov %0,%%cr3":: "b"((uint64_t)tempCr3):);
}



void pushInitialParamstoStack(Task* task)
{
    char** envp = (char**)kmalloc();
    
    envp[0] = (char*)"PWD=/";
    envp[1] = (char*)"PATH=/:/bin:/usr";
    envp[2] = NULL;
    
    char** args = (char**)kmalloc();
    
    args[0] = (char*)"ls";
    args[1] = (char*)"-l";
    args[2] = (char*)"-r";
    args[3] = NULL;
    
    
    uint64_t mainArgs = (uint64_t)kmalloc();
    mainArgs-=get_kernbase();
    mapPageForUser(0x300000,(uint64_t)mainArgs,task->regs.cr3+get_kernbase());
    mainArgs+=(get_kernbase());
    mainArgs+=0x1000;
    uint64_t tempMainArgs = mainArgs;
    
    uint64_t envStart = 0x302000;
    int i=0;
    
    pushSomeArgsToUser(mainArgs,NULL,task->regs.cr3);
    mainArgs-=8;
    
    while(((char**)envp)[i]!=NULL)
    {
        char* newPage = (char*)kmalloc();
        int k=0;
        int j=0;
        while(((char**)envp)[i][j]!='\0'&&k<=511) //only 512 chars
        {
            newPage[k] = ((char**)envp)[i][j];
            k++;
            j++;
        }
        newPage[k] = '\0';
        newPage-=get_kernbase();
        mapPageForUser(envStart,(uint64_t)newPage,task->regs.cr3+get_kernbase());
        envStart+=0x1000;
        i+=1;
    }
    i=envStart-0x1000;

    int z = 0x322000;
    
    while(z>=0x302000)
    {
        if(z==i)
        {
            pushSomeArgsToUser(mainArgs,(void*)(uint64_t)i,task->regs.cr3);
            i-=0x1000;
        }
        else
        {
            //pushSomeArgsToUser(task->regs.userRsp,(void*)0,task->regs.cr3);
        }
        
        mainArgs-=8;
        z-=0x1000;
    }
    
    pushSomeArgsToUser(mainArgs,NULL,task->regs.cr3);
    mainArgs-=8;
    
    char* newPage = (char*)kmalloc();
    
    i=0;
    int k=0;
    
   
    while(((char**)args)[i]!=NULL) i++;
    
    kprintf("ARGV COUNT %d",i);
    
    int count=i;
    
    i--;
    
    while(i>=0)
    {
        int j=0;
        
        pushSomeArgsToUser(mainArgs,(void*)(uint64_t)k+0x301000,task->regs.cr3);
        mainArgs-=8;
        
        while(((char**)args)[i][j]!='\0'&&k<=510)
        {
            newPage[k] = ((char**)args)[i][j];
            k++;
            j++;
        }
        newPage[k] = '\0';
        k++;
        i-=1;
    }
    
    newPage-=get_kernbase();
    mapPageForUser(0x301000,(uint64_t)newPage,task->regs.cr3+get_kernbase());
    
    pushSomeArgsToUser(mainArgs,(void*)(uint64_t)count,task->regs.cr3);
    mainArgs-=8;
    
    kprintf("%p %p\n\n",tempMainArgs,mainArgs);
    
    pushSomeArgsToUser(task->regs.userRsp,(void*)(uint64_t)0x301000-(tempMainArgs-mainArgs)-0x8,task->regs.cr3);
    task->regs.userRsp-=8;
    
    
    VMA* newVma = (VMA*)kmalloc();
    newVma->pageNumber = getNextPageNum();
    newVma->v_mm = &runningThread->memMap;
    newVma->v_start = 0x300000;
    newVma->v_end = 0x322000;
    newVma->mmsz = 0x22000;
    newVma->v_flags = 0;
    newVma->grows_down = 0;
    newVma->v_file = 0;
    newVma->next=NULL;
    newVma->v_offset=0;
    
    newVma->next = task->memMap.mmap;
    task->memMap.mmap = newVma;
    
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
    task->state=1;
    //This portion code is temporary : must be removed;
    task->endHH = 0;
    task->endMM = 0;
    task->endSS = 0;
    
   pushInitialParamstoStack(task);

//    pushSomeArgsToUser(task->regs.userRsp,0,cr3);
//    task->regs.userRsp-=8;
//    pushSomeArgsToUser(task->regs.userRsp,0,cr3);
//    task->regs.userRsp-=8;
//
//    pushSomeArgsToUser(task->regs.userRsp,0,cr3);
//    task->regs.userRsp-=8;

    //------------------------------------

    task->regs.kernelRsp=(uint64_t)kmalloc()+0x1000; // creating a stack for the kernel code of the user process
    task->regs.rbp=task->regs.userRsp; //doing this because rbp is base pointer of stack.
    task->regs.count=0;
    task->regs.add=0;
    task->next=0;
    //task->memMap.mmap=((void *)0);
    _prepareInitialKernelStack(&task->regs);
}



void createNewExecTask(Task *task,uint64_t function, uint64_t rflags,uint64_t cr3){
    
    task->pid_t = runningThread->pid_t;
    pidCount=0; //next process takes the next id
    task->ppid_t = 0; //setting the Pid of the parent for COW
    task->regs.rax=0;
    task->regs.rbx=0;
    task->regs.rcx=0;
    task->regs.rdx=0;
    task->regs.rsi=0;
    task->regs.rdi=0;
    task->regs.rflags=rflags;
    task->regs.rip=(uint64_t)function;
//    task->regs.cr3=cr3;
//    task->regs.userRsp=(uint64_t)stackForUser(task)+0x1000;   // creating a stack for the user process
//    pushSomeArgsToUser(task->regs.userRsp);
//    task->regs.userRsp-=8;
    task->regs.kernelRsp=(uint64_t)kmalloc()+0x1000; // creating a stack for the kernel code of the user process
    task->regs.rbp=task->regs.userRsp; //doing this because rbp is base pointer of stack.
    task->regs.count=0;
    task->regs.add=0;
    task->next=0;
    task->state=1; // 1 : is running
    task->endHH = 0;
    task->endMM = 0;
    task->endSS = 0;
    task->fd_pointers[0]=(uint64_t)kmalloc();
    for(int i=1;i<20;i++){
      task->fd_pointers[i]=0;
    }
    task->fd_count=5;
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
    
    Task* nextTask = runningThread->next;
    
    while(nextTask->state==0) nextTask=nextTask->next;
    
    runningThread = nextTask;
    
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
    
    invlpg(v_addr);
    
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

void init_terminal(){//TODO: ***Very Important::::SBUSH Taske atmost 64 characters of command, stdin takes 512 characters at max.. Handle that correctly..*****
    set_input_buf((uint64_t)kmalloc());
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
    init_terminal();
    _switchToRingThree(&last->regs, &runningThread->regs);
    
}


int64_t getPhysicalPageAddr(uint64_t v_addr,uint64_t cr3){
    
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
                int64_t phyAdr = getPhysicalPageAddr(start,runningThread->regs.cr3);
                if(phyAdr!=-1 && phyAdr!=0)
                {
                    mapPageForUser(start&FRAME,phyAdr,task->regs.cr3+get_kernbase());
                    markPageAsRW(start&FRAME,task->regs.cr3+get_kernbase(),1);
                }
                start = start + 0x1000;
            }
            else{
                uint64_t phy_child_stack = (uint64_t)kmalloc();
                phy_child_stack-=get_kernbase();
                mapPageForUser(start&FRAME,phy_child_stack,task->regs.cr3+get_kernbase());
                memcpy((void *)(start&FRAME),(void *)(phy_child_stack+get_kernbase()),4096);
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
    task->state = 1;
    task->exeName = runningThread->exeName;
    task->startHH = getCurHr();
    task->startMM = getCurMin();
    task->startSS = getCurSec();
    task->endHH = 0;
    task->endMM = 0;
    task->endSS = 0;
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


void addToQueue(Task* task)
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
    
    addToQueue(child);
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

void* printMe(void* a,void* b,void* c)
{
    kprintf("%d %s %s\n",(uint64_t)a,((char**)b)[0],((char**)c)[0]);//,(char*)args,(char*)envp);//,((char**)envp)[2]);
    kprintf("%d %s %s\n",(uint64_t)a,((char**)b)[1],((char**)c)[1]);
    kprintf("%d %s %s\n",(uint64_t)a,((char**)b)[2],((char**)c)[1]);
    return 0;
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
            if(!temp->next->grows_down)
                end = ((temp->next->v_end)>end?(temp->next->v_end):end);
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

void* exec(void* path,void* args,void* envp)
{
    
    /*kprintf("%s %s %s\n",((char*)path),((char**)args)[0],((char**)envp)[0]);
    kprintf("%s %s %s\n",((char*)path),((char**)args)[1],((char**)envp)[1]);
    kprintf("%s %s %s\n",((char*)path),((char**)args)[2],((char**)envp)[1]);
    kprintf("%s %s %s\n",((char*)path),((char**)args)[3],((char**)envp)[0]);
    kprintf("%s %s %s\n",((char*)path),((char**)args)[4],((char**)envp)[1]);
    kprintf("%s %s %s\n",((char*)path),((char**)args)[5],((char**)envp)[1]);*/
    int ia=0;
    if((uint64_t)args==0 || (uint64_t)envp==0){
        while(ia<100){
          ia++;
        }
    }
    Task *task = (Task*)kmalloc();
    uint64_t newCr3 = (uint64_t)getNewPML4ForUser();
    task->regs.cr3=newCr3;
    task->regs.userRsp=(uint64_t)stackForUser(task)+0x1000;
    
    uint64_t entryPoint = (loadFile(((char*)path),(newCr3+get_kernbase()),task));
    
    if(entryPoint==0)
    {
        pageDeAllocator((void*)((uint64_t)(task - get_kernbase())));
        return (void*)-1;
    }
    
 
    uint64_t mainArgs = (uint64_t)kmalloc();
    
    mainArgs-=get_kernbase();
    mapPageForUser(0x300000,(uint64_t)mainArgs,task->regs.cr3+get_kernbase());
    mainArgs+=(get_kernbase());
    mainArgs+=0x1000;
    
    uint64_t tempMainArgs = mainArgs;
    
    
    uint64_t envStart = 0x302000;
    int i=0;
    
    pushSomeArgsToUser(mainArgs,NULL,task->regs.cr3);
    mainArgs-=8;
    
    while(((char**)envp)[i]!=NULL)
    {
        char* newPage = (char*)kmalloc();
        int k=0;
        int j=0;
        while(((char**)envp)[i][j]!='\0'&&k<=511) //only 512 chars
        {
            newPage[k] = ((char**)envp)[i][j];
            k++;
            j++;
        }
        newPage[k] = '\0';
        newPage-=get_kernbase();
        mapPageForUser(envStart,(uint64_t)newPage,task->regs.cr3+get_kernbase());
        envStart+=0x1000;
        i+=1;
    }
    i=envStart-0x1000;
    
    int z = 0x322000;
    
    while(z>=0x302000)
    {
        if(z==i)
        {
            pushSomeArgsToUser(mainArgs,(void*)(uint64_t)i,task->regs.cr3);
            i-=0x1000;
        }
        else
        {
            //pushSomeArgsToUser(task->regs.userRsp,(void*)0,task->regs.cr3);
        }
        
        mainArgs-=8;
        z-=0x1000;
    }
    
    pushSomeArgsToUser(mainArgs,NULL,task->regs.cr3);
    mainArgs-=8;
    
    char* newPage = (char*)kmalloc();
    
    i=0;
    int k=0;
    
    
    while(((char**)args)[i]!=NULL) i++;
    
    kprintf("ARGV COUNT %d",i);
    
    int count=i;
    
    i--;
    
    while(i>=0)
    {
        int j=0;
        
        pushSomeArgsToUser(mainArgs,(void*)(uint64_t)k+0x301000,task->regs.cr3);
        mainArgs-=8;
        
        while(((char**)args)[i][j]!='\0'&&k<=510)
        {
            newPage[k] = ((char**)args)[i][j];
            k++;
            j++;
        }
        newPage[k] = '\0';
        k++;
        i-=1;
    }
    
    newPage-=get_kernbase();
    mapPageForUser(0x301000,(uint64_t)newPage,task->regs.cr3+get_kernbase());
    
    pushSomeArgsToUser(mainArgs,(void*)(uint64_t)count,task->regs.cr3);
    mainArgs-=8;
    
    pushSomeArgsToUser(task->regs.userRsp,(void*)(uint64_t)0x301000-(tempMainArgs-mainArgs)-0x8,task->regs.cr3);
    task->regs.userRsp-=8;
    
    VMA* newVma = (VMA*)kmalloc();
    newVma->pageNumber = getNextPageNum();
    newVma->v_mm = &runningThread->memMap;
    newVma->v_start = 0x300000;
    newVma->v_end = 0x321000;
    newVma->mmsz = 0x21000;
    newVma->v_flags = 0;
    newVma->grows_down = 0;
    newVma->v_file = 0;
    newVma->next=NULL;
    newVma->v_offset=0;
    
    newVma->next = task->memMap.mmap;
    task->memMap.mmap = newVma;
    
    kprintf("Entry Point: %p\n",entryPoint);
    createNewExecTask(task,entryPoint,runningThread->regs.rflags,newCr3);
    task->exeName = "bin/ps";
    task->startHH = getCurHr();
    task->startMM = getCurMin();
    task->startSS = getCurSec();
    addToQueue(task);
    
    
    Task tempTask = runningThread;
    
    while(tempTask->next!=runningThread)
        tempTask=tempTask->next;
    
    tempTask->next = runningThread->next;
    
    runningThread=runningThread->next;
    runNextTask();
    
    
    return 0;
}


void* waitpid(void* pid,void* status,void* flags)
{
    
    kprintf("%d %d %d\n",(uint64_t)pid,(uint64_t*)status,(uint64_t)flags);
    
    int* temp = (int*)status;
    
    Task* tempTask = runningThread->next;
    
    while(tempTask!=runningThread)
    {
        if( tempTask->pid_t==(uint64_t)pid && tempTask->state == 1 )
        {
            *temp=1;
            return (void*)(uint64_t)pid;
            
        }
        tempTask=tempTask->next;
    }
    
    //adsdasd
    *temp = 0;
    return (void*)(-1);
}

void* free(void* ptr)
{
    
    kprintf("PTR To Free\n",(uint64_t)ptr);
    
    uint64_t pageToDel = (uint64_t)ptr&FRAME;
    
    Task* task = runningThread;
    
    VMA* vmaTemp = task->memMap.mmap;
    
    int count=0;
    
    while(vmaTemp!=NULL)
    {
        if(vmaTemp->v_start>=pageToDel && vmaTemp->v_end<=(pageToDel+0x1000))
            count+=1;
        
        vmaTemp=vmaTemp->next;
        
    }
    
    vmaTemp = task->memMap.mmap;
    
    if(vmaTemp->v_start==(uint64_t)ptr)
        task->memMap.mmap = vmaTemp->next;
    
    VMA* eTemp = vmaTemp;
    
    while(vmaTemp->next!=NULL)
    {
        if(vmaTemp->next->v_start==(uint64_t)ptr)
        {
            eTemp->next = vmaTemp->next->next;
            //pageDeAllocator((void*)((uint64_t)(vmaTemp->next - get_kernbase())&FRAME));
            break;
        }
        eTemp=vmaTemp->next;
        vmaTemp = vmaTemp->next;
        
    }
    
    
    int pageCount=1;
    
    //find the physical Address and check if the page is shared.
    int64_t phyAddr = getPhysicalPageAddr(pageToDel,task->regs.cr3);
    
    if(phyAddr!=-1 && phyAddr!=0)
    {
        //before deleting the pagees we need to check if the page is shared
        
        
        phyAddr&=FRAME;
        
        Task* tempTask = task->next;
        
        while(tempTask!=task)
        {
            int64_t tempPhy = getPhysicalPageAddr(phyAddr,tempTask->regs.cr3);
            if(tempPhy!=-1 && tempPhy!=0 && (tempPhy&FRAME)==phyAddr)
                pageCount+=1; //how many cr3 contain this phy address,virtual address combo
            tempTask=tempTask->next;
        }
        
    }
    
    //if(count==1&&pageCount==1)
        //pageDeAllocator((void*)(phyAddr&FRAME));
    
    
    
    
    return 0;
}

void* ps()
{
    
    Task * task = runningThread;
    
    char * temp;
    if(task->state==1) temp = "RUNNING";
    else temp = "NOT-RUNNING";
    
    kprintf("PID START_TIME END_TIME STATE      BINARY\n");
    kprintf("%d  %d:%d:%d   %d:%d:%d %s         %s\n",task->pid_t,task->startHH,task->startMM,task->startSS,task->endHH,task->endMM,task->endSS,temp,task->exeName);
    
    while(task->next!=runningThread)
    {
        if(task->state==1) temp = "RUNNING";
        else temp = "NOT-RUNNING";
        
        
        kprintf("%d    %d:%d:%d    %d:%d:%d    %s      %s\n",task->pid_t,task->startHH,task->startMM,task->startSS,task->endHH,task->endMM,task->endSS,temp,task->exeName);
        
        
        task=task->next;
    }
    return 0;
}


void FreePageEntries(Task* task)
{
    VMA* tempVMA = task->memMap.mmap;
    
    while(tempVMA!=NULL)
    {
        uint64_t start = tempVMA->v_start;
    
        while(start<tempVMA->v_end)
        {
            int64_t phyAddr = getPhysicalPageAddr(start,task->regs.cr3);
            
            if(phyAddr!=-1 && phyAddr!=0)
            {
                //before deleting the pagees we need to check if the page is shared
                int pageCount=1;
               
                phyAddr&=FRAME;
                
                Task* temp = task->next;
                
                while(temp!=task)
                {
                    int64_t tempPhy = getPhysicalPageAddr(phyAddr,temp->regs.cr3);
                    if(tempPhy!=-1 && tempPhy!=0 && (tempPhy&FRAME)==phyAddr)
                        pageCount+=1; //how many cr3 contain this phy address,virtual address combo
                    temp=temp->next;
                }
                
                if(pageCount==1)
                {
                    
                    kdealloc((void*)((phyAddr&FRAME)+get_kernbase()));
                    
                }
                else
                {
                    //skip free
                    
                }
                
            }
            
            start+=0x1000;
        }
        tempVMA=tempVMA->next;
    }
}


void FreePageTables(Task* task)
{
    uint64_t* pml4 = (uint64_t*)(task->regs.cr3+get_kernbase());
    for(int i=0;i<511;i++)
    {
        if(pml4[i]&PRESENT)
        {
            uint64_t *pdpt = (uint64_t*)(pml4[i]+get_kernbase());
            for(int j=0;j<512;j++)
            {
                if(pdpt[j]&PRESENT)
                {
                    uint64_t *pdt = (uint64_t*)(pdpt[j]+get_kernbase());
                    for(int k=0;k<512;k++)
                    {
                        if(pdt[k]&PRESENT)
                        {
                            pageDeAllocator((void*)(pdt[k]&FRAME));
                        }
                    }
                    pageDeAllocator((void*)(pdpt[j]&FRAME));
                }
                
            }
            pageDeAllocator((void*)(pml4[i]&FRAME));
                
        }
    
    }
    
}

void* exit(void* pid)
{
    //delete the task and free all memory
    Task* task = runningThread;
    
//    if((uint64_t)pid == 0) task=runningThread;
//    else
//    {
//        if(runningThread->pid_t==(uint64_t)pid) task = runningThread;
//        else
//        {
//            task=runningThread->next;
//            while(task!=runningThread)
//            {
//                if(task->pid_t==(uint64_t)pid && task->state!=0) break;
//                task = task->next;
//            }
//
//        }
//    }
    
    while(task->next!=runningThread)
        task=task->next;
    
    task->next = runningThread->next;
    
    runningThread=runningThread->next;
    
    
   // Task* task = runningThread;
    task->state = 0;
    task->endHH = getCurHr();
    task->endMM = getCurMin();
    task->endSS = getCurSec();
    //FreePageEntries(task);
    //FreePageTables(task);
    //while(1);
    runNextTask();
    return 0;
}

void initUserProcess()
{
    
    uint64_t U2_cr3 = (uint64_t)getNewPML4ForUser();
    Task *userThread2 = (Task*)kmalloc();
    uint64_t hello_entrypoint = (loadFile("bin/sbush",(U2_cr3+get_kernbase()),userThread2));
    
    if(hello_entrypoint == 0) return;
    
    kprintf("Entry Point: %p\n",hello_entrypoint);
    userThread2->exeName = "bin/sbush";
    
    
    createNewTask(userThread2,hello_entrypoint,mainThread.regs.rflags,U2_cr3);
    
    mainThread.next = userThread2;
    userThread2->next = userThread2;
    
    enableIntr();
    
    userThread2->startHH = getCurHr();
    userThread2->startMM = getCurMin();
    userThread2->startSS = getCurSec();
    
    
    switchToUserMode();
    
}

void* clearScreen()
{
    
    for(int i=0;i<30;i++) kprintf("\n");
    return 0;
    
}
