#include <sys/defs.h>
#include <sys/gdt.h>
#include <sys/idt.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/ahci.h>
#include <sys/pci.h>
#include <sys/pit.h>
#include <sys/pic.h>
#include <sys/rtc.h>
#include<sys/phyMemMapper.h>
#include<sys/virtualMemory.h>
#include<sys/task.h>
#include<sys/syscall.h>
#include<sys/tarfs.h>

#define INITIAL_STACK_SIZE 4096
uint8_t initial_stack[INITIAL_STACK_SIZE]__attribute__((aligned(16)));
uint32_t* loader_stack;
extern char kernmem, physbase;

void start(uint32_t *modulep, void *physbase, void *physfree)
{

  initPhys((uint64_t)physbase,(uint64_t)physfree);
  allocateBitmapMem();
  allocateAHCI();
  struct smap_t {
    uint64_t base, length;
    uint32_t type;
  }__attribute__((packed)) *smap;
  while(modulep[0] != 0x9001) modulep += modulep[1]+2;
  for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
    if (smap->type == 1 /* memory */ && smap->length != 0) {
      kprintf("Available Physical Memory [%p-%p]\n", smap->base, smap->base + smap->length);
      initBitmap(smap->base,smap->base + smap->length);
    }
  }
  kprintf("physfree %p\n", (uint64_t)physfree);
  kprintf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);
  mapKernelMemory();
  printALLDrivers();
  enablePaging();
    
    
  kprintf("***************************Paging Enabled***************************\n");
  //probeAHCI();
  //initUserMode();
  initTarfs();
  //loadFile("rakshith",0xfffffffff);
  initMultiTasking();
  initSyscalls();
  //int fd = openDirectory("usr/");
  //char* buf=kmalloc();
  /*int fd2 = openFile("usr/folder1/file1.txt");
  fd2++;
  int fd3 = openFile("usr/folder1/file1.txt");
  fd3++;
  int fd4 = openFile("usr/folder1/file1.txt");
  readFile(fd4,buf,75);
  kprintf("%s\n",buf);
  closeFile(fd);
  readFile(fd4,buf,3);
  kprintf("%d\n",sizeof(struct fileDescriptor));
  kprintf("%d\n",sizeof(struct Task));*/
  //while(!readDir(fd,buf,4096)){
    //kprintf("%s\n",buf);
  //}
  //getDirEntries(fd,buf,4096);
  //struct dirent *dbuf=(struct dirent *)buf;
  //kprintf("%s",dbuf->d_name);
  initUserProcess();
  
    
    
    
    
    
    
  
  while(1);
}

void boot(void)
{
  // note: function changes rsp, local stack variables can't be practically used
  register char *temp1, *temp2;

  for(temp2 = (char*)0xb8001+160*24; temp2 < (char*)0xb8000+160*25; temp2 += 2) *temp2 = 14 /* white */;
  __asm__ volatile (
    "cli;"
    "movq %%rsp, %0;"
    "movq %1, %%rsp;"
    :"=g"(loader_stack)
    :"r"(&initial_stack[INITIAL_STACK_SIZE])
  );
  init_gdt();
  init_idt();
    
 init_pic();
 init_pit();
 init_rtc();
    
   
   
    
  keypress_bar("Last Pressed: ",0XF0);
  boot_time_bar(0,0,0,0XF0);
  time_bar(0,0,0,0XF0);
  

  
    //__asm__ __volatile__ ("int $0x10":::);
  
  start(
    (uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
    (uint64_t*)&physbase,
    (uint64_t*)(uint64_t)loader_stack[4]
  );
  for(
    temp1 = "!!!!! start() returned !!!!!", temp2 = (char*)0xb8000;
    *temp1;
    temp1 += 1, temp2 += 2
  ) *temp2 = *temp1;
    
  while(1) __asm__ volatile ("hlt");
}
