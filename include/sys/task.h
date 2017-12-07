#ifndef _TASK_H
#define _TASK_H
#include<dirent.h>
#include<sys/defs.h>

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
    uint64_t pageNumber;
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


typedef struct Task{
  /*int  priority;
  int  state;*/  //TODO: these two variables might be used later	
  int pid_t;
  int ppid_t;
    int startHH;
    int startMM;
    int startSS;
    
    int endHH;
    int endMM;
    int endSS;
    
    char exeName[15];
    
  Registers regs;
  struct Task *next;
  uint64_t state;
  struct mm_struct memMap;
  uint64_t fd_pointers[20];//Each page contains 5 FD's.. Each entry of this page is the starting adress of the pages allocated for FD's. Initialize fd[0] with new page.
  int fd_count;//Need to initialize this with 5. If user requests for 6th then, a page will be allocated and fd_pointers[1] will be filled with that adress.
} Task;


void* free(void* ptr);
void* ps();
void* exit(void* num);
void runNextTask();
void initMultiTasking();
void createThread(Task* task, void(*function)(), uint64_t rflags, uint64_t *pml4);
void yield();
void _switchThread_(Registers *from, Registers *to);
void createNewTask(Task *kthread, uint64_t function, uint64_t rflags,uint64_t cr3);
void switchToUserMode();
uint64_t fork();
uint64_t malloc(uint64_t size);
int64_t getPhysicalPageAddr(uint64_t v_addr,uint64_t cr3);
void* exec(void* path,void* args,void* envp);
void addCurrentTasktoRunQueue(Task* task);
void markPagesAsReadOnly(uint64_t cr3);
Task* getRunTask();
int isPartofCurrentVma(uint64_t addr);
uint64_t getNextPageNum();
uint64_t getPageNumFromAddr(uint64_t addr);
int markPageAsRW(uint64_t v_addr,uint64_t cr3,int rw);
void makePageCopiesForChilden(uint64_t pNum,Task* task);
Task* getRunningThread();
void init_terminal();
void* printMe(void* path,void* args,void* envp);
void* waitpid(void* pid,void* status,void* flags);
void* enableSleep();

void* getCounter();
void* disableSleep();
void* kill(void* pid);

void* clearScreen();

#endif
