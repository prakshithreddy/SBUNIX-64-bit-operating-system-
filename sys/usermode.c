#include<sys/usermode.h>
#include<sys/defs.h>
#include<sys/virtualMemory.h>
#include<sys/phyMemMapper.c>
#include<sys/kprintf.h>

#define GDT_CS        (0x00180000000000)  /*** code segment descriptor ***/
#define GDT_DS        (0x00100000000000)  /*** data segment descriptor ***/

#define C             (0x00040000000000)  /*** conforming ***/
#define DPL0          (0x00000000000000)  /*** descriptor privilege level 0 ***/
#define DPL1          (0x00200000000000)  /*** descriptor privilege level 1 ***/
#define DPL2          (0x00400000000000)  /*** descriptor privilege level 2 ***/
#define DPL3          (0x00600000000000)  /*** descriptor privilege level 3 ***/
#define P             (0x00800000000000)  /*** present ***/
#define L             (0x20000000000000)  /*** long mode ***/
#define D             (0x40000000000000)  /*** default op size ***/
#define W             (0x00020000000000)  /*** writable data segment ***/

static kernelThread mainThread;

uint64_t getCr3()
{
    uint64_t cr3;
    __asm__ __volatile__("movq %%cr3,%0" : "=r" (saved_cr3));
    return cr3;
}

void* getNewPML4ForUser()
{
    struct PML4 *newPML4=(struct PML4*)pageAllocator();
    struct PML4 *currPML4=(struct PML4*)getCr3();
    //get the current pml4 virtual address to copy the kernel page table
    currPML4=(struct PML4*)((uint64_t)currPML4+kernbase);
    ((struct PML4*)((uint64_t)newPML4+kernbase))->entries[511]=currPML4->entries[511];
    return (void*)newPML4;
}

void setCr3(struct PML4* cr3Addr)
{
    uint64_t temp = (uint64_t)cr3Addr;
    __asm__ __volatile__("movq %0,%%cr3"::"b"(temp));
}


void initUserMode()
{
    __asm__ __volatile__("movq %%cr3, %%rax; movq %%rax, %0;":"=m"(mainThread.regs.cr3)::"%rax");
    __asm__ __volatile__("pushfq; movq (%%rsp), %%rax; movq %%rax, %0; popfq;":"=m"(mainThread.regs.rflags)::"%rax");
    
    kernelThread userThread = (kernelThread*)kmalloc();
    userThread->regs.rax=0;
    userThread->regs.rbx=0;
    userThread->regs.rcx=0;
    userThread->regs.rdx=0;
    userThread->regs.rsi=0;
    userThread->regs.rdi=0;
    userThread->regs.rflags=rflags;
    userThread->regs.rip=(uint64_t)function;
    userThread->regs.cr3=(uint64_t)getNewPML4ForUser();
    userThread->regs.rsp=(uint64_t)kmalloc()+0x1000;
    userThread->regs.rbp=userThread->regs.rsp; //doing this because rbp is base pointer of stack.
    userThread->next=0;
    
    
}


