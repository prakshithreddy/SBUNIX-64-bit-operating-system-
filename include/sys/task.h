#ifndef _TASK_H
#define _TASK_H

#include<sys/defs.h>

typedef struct {
    uint64_t rax, rbx, rcx, rdx, rsi, rdi, userRsp, rbp, rip, rflags, cr3,kernelRsp;
} Registers;

uint64_t* currentRsp;

void initUserProcess();

typedef struct Task {
  /*int  priority;
  int  state;*/  //TODO: these two variables might be used later.
  Registers regs;
  struct Task *next;
} Task;

/*typedef struct thread {
   process*  parent;
   void*     initialStack;
   void*     stackLimit;
   void*     kernelStack;
   uint32_t  priority;
   int       state;
   Registers registers;
}thread;

typedef struct process {
  int            id;
  int            priority;
  pdirectory*    pageDirectory;
  int            state;
  typedef struct _process* next;
  thread* threadList;
  thread  threads[MAX_THREAD];
  
}process;*//*TODO: we can add more information, such as the following:  -LDT descriptor [if used]  -User and kernel times  -Execution options, etc*/


void initMultiTasking();
void createThread(Task* task, void(*function)(), uint64_t rflags, uint64_t *pml4);
void yield();
void _switchThread_(Registers *from, Registers *to);

void switchToUserMode();

#endif
