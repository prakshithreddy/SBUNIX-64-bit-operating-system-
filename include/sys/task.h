#ifndef _TASK_H
#define _TASK_H

#include<sys/defs.h>
#include<dirent.h>
typedef struct {
    uint64_t rax, rbx, rcx, rdx, rsi, rdi, userRsp, rbp, rip, rflags, cr3,kernelRsp,count,add;
} Registers;

uint64_t* getRunKRsp();
uint64_t getRunCr3();
uint64_t* currentRsp;
uint64_t currentRAX;

void initUserProcess();

typedef struct vma_struct{                                                                                   
    struct mm_struct *v_mm;// associated mm_struct
    uint64_t v_start;//VMA start, inclusive
    uint64_t v_end;//VMA end, exclusive
    uint64_t mmsz;//VMA size
    uint64_t v_flags;//flags
    uint64_t grows_down;//Indicator for stack
    uint64_t v_file;//mapped file, if any
    struct vma_struct  *next;//list of VMA's 
    uint64_t v_offset;//file offset  
}VMA;


typedef struct mm_struct {
    int count;
    uint64_t *pt; //page table pointer  
    uint64_t context;
    uint64_t start_code, end_code, start_data, end_data;
    uint64_t start_brk, brk, start_stack, start_mmap;
    uint64_t arg_start, arg_end, env_start, env_end;
    uint64_t rss, total_vm, locked_vm;
    uint64_t def_flags;
    VMA *mmap;
    VMA * mmap_avl;
}MM;


typedef struct Task {
  /*int  priority;
  int  state;*/  //TODO: these two variables might be used later	
  int pid_t;
  int ppid_t;
  Registers regs;
  struct Task *next;
  struct mm_struct memMap;
  File files[50];
  Directory directories[50];
} Task;

void runNextTask();
void initMultiTasking();
void createThread(Task* task, void(*function)(), uint64_t rflags, uint64_t *pml4);
void yield();
void _switchThread_(Registers *from, Registers *to);
void createNewTask(Task *kthread, uint64_t function, uint64_t rflags,uint64_t cr3,int parentPid);
void switchToUserMode();
void fork(Task* parent);

#endif
