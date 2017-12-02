#include<sys/virtualMemory.h>
#include<sys/phyMemMapper.h>
#include<sys/kprintf.h>
#define VGA_ADDRESS 0xb8000
#define AHCI_VIRTUAL_ADDRESS 0xFFFFFFFF80050000
#define USER_VIRTUAL_STACK_TOP 0xFFFFFF7F00000000
static uint64_t virtual_physbase=(uint64_t)&kernmem; //virtual_kernel_address -> pointer to free virtual adress above kernmem for usage in kernel.
static uint64_t phys_base=(uint64_t)&physbase;
static uint64_t kernbase;
static uint64_t vga_virtual_address;
//static uint64_t vga_end_virtual_address;
static struct PML4 *pml4;

uint64_t get_kernbase(){
  return kernbase;
}

uint64_t get_stack_top(){
  return USER_VIRTUAL_STACK_TOP;
}

void mapKernelMemory(){
  //uint64_t physbase = get_ker_physbase();
  //uint64_t physfree = get_ker_physfree();
  pml4 = (struct PML4*)pageAllocator();
  memset((uint64_t)pml4);
  struct PDPT *pdpt = (struct PDPT*)pageAllocator();
  memset((uint64_t)pdpt);
  struct PDT *pdt = (struct PDT*)pageAllocator();
  memset((uint64_t)pdt);
  struct PT *pt = (struct PT*)pageAllocator();
  memset((uint64_t)pt);
  
  //creating pdpt to map to pml4
  uint64_t pdpt_e = (uint64_t)pdpt;
  pdpt_e|=PRESENT;
  pdpt_e|=WRITEABLE;
  //pdpt_e|=USER;
  pml4->entries[get_PML4_INDEX((uint64_t)&kernmem)]=pdpt_e;
  
  //creating pdt to map to pdpt
  uint64_t pdt_e = (uint64_t)pdt;
  pdt_e|=PRESENT;
  pdt_e|=WRITEABLE;
  //pdt_e|=USER;
  pdpt->entries[get_PDPT_INDEX((uint64_t)&kernmem)]=pdt_e;
  
  //creating pdt to map to pdpt
  uint64_t pt_e = (uint64_t)pt;
  pt_e|=PRESENT;
  pt_e|=WRITEABLE;
  //pt_e|=USER;
  pdt->entries[get_PDT_INDEX((uint64_t)&kernmem)]=pt_e;
  
  //physfree+=0x5000; //TODO: no idea why additonal memory is added.. need to verify
  
  //mapping from physbase to physfree will be done in identityMapping Function.
  /*while(physbase<physfree){
    uint64_t entry=physbase;
    entry|=PRESENT;
    entry|=WRITEABLE;
    entry|=USER;
    pt->entries[get_PT_INDEX(virtual_kaddr)]=entry;
    physbase+=0x1000;
    virtual_kaddr+=0x1000;
  }*/
  
  identityMapping();
  vga_virtual_address=kernbase+0xb8000;
  //vga_end_virtual_address = vga_virtual_address+0x3000;
  //kprintf("PML4 before enabling Paging %p\n",pml4->entries[511]);
  mapVideoMemory(vga_virtual_address);
}

void mapPage(uint64_t v_addr, uint64_t phy_addr){
  struct PDPT *pdpt;
  struct PDT *pdt;
  struct PT *pt;
  
  uint64_t pml_entry = pml4->entries[get_PML4_INDEX((uint64_t)v_addr)];
  if(pml_entry&PRESENT){
    pdpt = (struct PDPT*)(pml_entry&FRAME);
  }
  else{
    pdpt = (struct PDPT*)pageAllocator();
    memset((uint64_t)pdpt);
    pml_entry = (uint64_t)pdpt;
    pml_entry|=PRESENT;
    pml_entry|=WRITEABLE;
    //pml_entry|=USER;
    pml4->entries[get_PML4_INDEX((uint64_t)v_addr)]=pml_entry;
  }
  
  uint64_t pdpt_entry = pdpt->entries[get_PDPT_INDEX((uint64_t)v_addr)];
  if(pdpt_entry&PRESENT){
    pdt = (struct PDT*)(pdpt_entry&FRAME);
  }
  else{
    pdt = (struct PDT*)pageAllocator();
    memset((uint64_t)pdt);
    pdpt_entry = (uint64_t)pdt;
    pdpt_entry|=PRESENT;
    pdpt_entry|=WRITEABLE;
    //pdpt_entry|=USER;
    pdpt->entries[get_PDPT_INDEX((uint64_t)v_addr)]=pdpt_entry;
  }
  
  uint64_t pdt_entry = pdt->entries[get_PDT_INDEX((uint64_t)v_addr)];
  if(pdt_entry&PRESENT){
    pt = (struct PT*)(pdt_entry&FRAME);
  }
  else{
    pt = (struct PT*)pageAllocator();
    memset((uint64_t)pt);
    pdt_entry = (uint64_t)pt;
    pdt_entry|=PRESENT;
    pdt_entry|=WRITEABLE;
    //pdt_entry|=USER;
    pdt->entries[get_PDT_INDEX((uint64_t)v_addr)]=pdt_entry;
  }
  
  uint64_t pt_entry = phy_addr;
  pt_entry|=PRESENT;
  pt_entry|=WRITEABLE;
  //pt_entry|=USER;
  pt->entries[get_PT_INDEX((uint64_t)v_addr)]=pt_entry;
  
}

void forceMapPage(uint64_t v_addr, uint64_t phy_addr,uint64_t temp,int user){//TODO: This function maps physical and virtual in the given pml4 table..   
                                                                                        //user=1 implies user bit will be set.
  struct PDPT *pdpt;//TODO: This function is used only after enabling paging.
  struct PDT *pdt;
  struct PT *pt;
  struct PDPT *v_pdpt;
  struct PDT *v_pdt;
  struct PT *v_pt;
  
  struct PML4 *curr_pml4=(struct PML4*)temp;//temp that is passed is already virtual..
  
  uint64_t pml_entry = curr_pml4->entries[get_PML4_INDEX((uint64_t)v_addr)];
  if(pml_entry&PRESENT){
    pdpt = (struct PDPT*)(pml_entry&FRAME);
  }
  else{
    pdpt = (struct PDPT*)pageAllocator();
    uint64_t vir_pdpt = (uint64_t)pdpt+kernbase;
    memset((uint64_t)vir_pdpt);
    pml_entry = (uint64_t)pdpt;
    pml_entry|=PRESENT;
    pml_entry|=WRITEABLE;
    if(user){
    pml_entry|=USER;
    }
    curr_pml4->entries[get_PML4_INDEX((uint64_t)v_addr)]=pml_entry;
  }
  v_pdpt = (struct PDPT*)((uint64_t)pdpt+kernbase);
  uint64_t pdpt_entry = v_pdpt->entries[get_PDPT_INDEX((uint64_t)v_addr)];
  if(pdpt_entry&PRESENT){
    pdt = (struct PDT*)(pdpt_entry&FRAME);
  }
  else{
    pdt = (struct PDT*)pageAllocator();
    uint64_t vir_pdt = (uint64_t)pdt+kernbase;
    memset((uint64_t)vir_pdt);
    pdpt_entry = (uint64_t)pdt;
    pdpt_entry|=PRESENT;
    pdpt_entry|=WRITEABLE;
    if(user){
    pdpt_entry|=USER;
    }
    v_pdpt->entries[get_PDPT_INDEX((uint64_t)v_addr)]=pdpt_entry;
  }
  
  v_pdt = (struct PDT*)((uint64_t)pdt+kernbase);
  uint64_t pdt_entry = v_pdt->entries[get_PDT_INDEX((uint64_t)v_addr)];
  if(pdt_entry&PRESENT){
    pt = (struct PT*)(pdt_entry&FRAME);
  }
  else{
    pt = (struct PT*)pageAllocator();
    uint64_t vir_pt = (uint64_t)pt+kernbase;
    memset((uint64_t)vir_pt);
    pdt_entry = (uint64_t)pt;
    pdt_entry|=PRESENT;
    pdt_entry|=WRITEABLE;
    if(user){
    pdt_entry|=USER;
    }
    v_pdt->entries[get_PDT_INDEX((uint64_t)v_addr)]=pdt_entry;
  }
  
  v_pt = (struct PT*)((uint64_t)pt+kernbase);
  uint64_t pt_entry = phy_addr;
  pt_entry|=PRESENT;
  pt_entry|=WRITEABLE;
  if(user){
  pt_entry|=USER;
  }
  v_pt->entries[get_PT_INDEX((uint64_t)v_addr)]=pt_entry;
}

void mapPageForUser(uint64_t v_addr, uint64_t phy_addr,uint64_t temp){//TODO: This function maps physical and virtual in the given pml4 table.. 
  struct PDPT *pdpt;//TODO: This function is used only after enabling paging.
  struct PDT *pdt;
  struct PT *pt;
  struct PDPT *v_pdpt;
  struct PDT *v_pdt;
  struct PT *v_pt;
  
  struct PML4 *curr_pml4=(struct PML4*)temp;//temp that is passed is already virtual..
  
  uint64_t pml_entry = curr_pml4->entries[get_PML4_INDEX((uint64_t)v_addr)];
  if(pml_entry&PRESENT){
    pdpt = (struct PDPT*)(pml_entry&FRAME);
  }
  else{
    pdpt = (struct PDPT*)pageAllocator();
    uint64_t vir_pdpt = (uint64_t)pdpt+kernbase;
    memset((uint64_t)vir_pdpt);
    pml_entry = (uint64_t)pdpt;
    pml_entry|=PRESENT;
    pml_entry|=WRITEABLE;
    pml_entry|=USER;
    curr_pml4->entries[get_PML4_INDEX((uint64_t)v_addr)]=pml_entry;
  }
  v_pdpt = (struct PDPT*)((uint64_t)pdpt+kernbase);
  uint64_t pdpt_entry = v_pdpt->entries[get_PDPT_INDEX((uint64_t)v_addr)];
  if(pdpt_entry&PRESENT){
    pdt = (struct PDT*)(pdpt_entry&FRAME);
  }
  else{
    pdt = (struct PDT*)pageAllocator();
    uint64_t vir_pdt = (uint64_t)pdt+kernbase;
    memset((uint64_t)vir_pdt);
    pdpt_entry = (uint64_t)pdt;
    pdpt_entry|=PRESENT;
    pdpt_entry|=WRITEABLE;
    pdpt_entry|=USER;
    v_pdpt->entries[get_PDPT_INDEX((uint64_t)v_addr)]=pdpt_entry;
  }
  
  v_pdt = (struct PDT*)((uint64_t)pdt+kernbase);
  uint64_t pdt_entry = v_pdt->entries[get_PDT_INDEX((uint64_t)v_addr)];
  if(pdt_entry&PRESENT){
    pt = (struct PT*)(pdt_entry&FRAME);
  }
  else{
    pt = (struct PT*)pageAllocator();
    uint64_t vir_pt = (uint64_t)pt+kernbase;
    memset((uint64_t)vir_pt);
    pdt_entry = (uint64_t)pt;
    pdt_entry|=PRESENT;
    pdt_entry|=WRITEABLE;
    pdt_entry|=USER;
    v_pdt->entries[get_PDT_INDEX((uint64_t)v_addr)]=pdt_entry;
  }
  
  v_pt = (struct PT*)((uint64_t)pt+kernbase);
  uint64_t pt_entry = phy_addr;
  pt_entry|=PRESENT;
  pt_entry|=WRITEABLE;
  pt_entry|=USER;
  v_pt->entries[get_PT_INDEX((uint64_t)v_addr)]=pt_entry;
  
}

void enablePaging(){
  //kprintf("Before Paging\n");
  __asm__ __volatile__("mov %0,%%cr3":: "b"((uint64_t)pml4));
  //kprintf("After Paging\n");
  //struct PML4 *temp = (struct PML4 *)(kernbase+(uint64_t)pml4);
  //kprintf("PML4 after enabling Paging %p\n",temp->entries[511]);
  /*uint64_t temp=0;
  __asm__ __volatile__("mov %%cr0,%0":"=a"(temp):);
  temp|=1<<31;
  kprintf("CR0:- %p",temp);
  __asm__ __volatile__("mov %0,%%cr0":: "b"((uint64_t)temp));*/
  set_availFrames(kernbase);
  pml4= (struct PML4 *)((uint64_t)pml4+kernbase);
}

void identityMapping(){
  kernbase=virtual_physbase - phys_base;
  uint64_t vir_start=kernbase+PHYSSTART;
  uint64_t phys_start=PHYSSTART;
  uint64_t vir_end=0;
  uint64_t endOfMemory = 0xFFFFFFFFFFFFF000 - kernbase;
  if(get_physend()>endOfMemory)
  {
    vir_end = 0xFFFFFFFF88000000;
  }
  else vir_end=kernbase+get_physend();
  
  kprintf("Identity Mapping %p to %p, till %p\n",phys_start,vir_start,vir_end);
  while(vir_start<=vir_end){
    mapPage(vir_start,phys_start);
    vir_start+=0x1000;
    phys_start+=0x1000;
  }
  kprintf("Indentity done\n");
}

void mapVideoMemory(uint64_t vga_virtual_address){
  mapPage(vga_virtual_address,VGA_ADDRESS);
  mapVGA(vga_virtual_address);
}

uint64_t mapAHCI(uint64_t abar_phys){
  mapPage(AHCI_VIRTUAL_ADDRESS,abar_phys);
  mapPage(AHCI_VIRTUAL_ADDRESS+0x1000,abar_phys+0x1000);
  return AHCI_VIRTUAL_ADDRESS;
}
void* kmalloc(){
  void *ptr=pageAllocator();
  ptr= (void*)((uint64_t)ptr+kernbase);
  memset((uint64_t)ptr);
  return ptr;
}

void* stackForUser(Task *uthread){
    void *ptr=pageAllocator();
    mapPageForUser(USER_VIRTUAL_STACK_TOP-0x1000,(uint64_t)ptr,(uint64_t)(uthread->regs.cr3)+kernbase);
    forceMapPage(USER_VIRTUAL_STACK_TOP-0x1000,(uint64_t)ptr,(uint64_t)pml4,0);
    memset((uint64_t)ptr+kernbase);
    
    //Adding VMA STRUCT
    MM *mm = uthread->memMap;
    VMA *vma_start;
    VMA *new_vma;
    vma_start = mm->mmap;
    while(vma_start != NULL && vma_start->next != NULL){
        vma_start=vma_start->next;
    }
    new_vma=(VMA *)kmalloc();//TODO: Allocating a full page for just one VMA struct, which is too expensive, SOL1: Can assume that one page is sufficient 
                             //for all the mallocs that will be done by the user. SOL2: Find a new way.. :P
    if(vma_start==NULL){
        mm->mmap = new_vma;
    }
    else{
        vma_start->next=new_vma;
    }
    mm->count++;
    new_vma->next=NULL;
    new_vma->grows_down=1;
    new_vma->v_start=USER_VIRTUAL_STACK_TOP-0x1000;
    new_vma->v_end=USER_VIRTUAL_STACK_TOP;
    new_vma->mmsz=0x1000;
    //new_vma->v_flags=elf_p_hdr->p_flags;//TODO: So many other details can be stored in VMA struct, Not sure what flags to set to stack.
    
    //*************************END of creating VMA structs, Start of mapping memory..
    return (void *)(USER_VIRTUAL_STACK_TOP-0x1000);
}

uint64_t getCr3()
{
    uint64_t cr3;
    __asm__ __volatile__("movq %%cr3,%0" : "=r"(cr3));
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

