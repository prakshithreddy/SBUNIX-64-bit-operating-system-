#include<sys/virtualMemory.h>
#include<sys/phyMemMapper.h>
#include<sys/kprintf.h>
#define VGA_ADDRESS 0xb8000
static uint64_t vga_virtual_address;
static uint64_t vga_end_virtual_address;
static struct PML4 *pml4;

void mapKernelMemory(){
  uint64_t physbase = get_ker_physbase();
  uint64_t physfree = get_ker_physfree();
  pml4 = (struct PML4*)pageAllocator();
  struct PDPT *pdpt = (struct PDPT*)pageAllocator();
  struct PDT *pdt = (struct PDT*)pageAllocator();
  struct PT *pt = (struct PT*)pageAllocator();
  
  //creating pdpt to map to pml4
  uint64_t pdpt_e = (uint64_t)pdpt;
  pdpt_e|=PRESENT;
  pdpt_e|=WRITEABLE;
  pdpt_e|=USER;
  pml4->entries[get_PML4_INDEX((uint64_t)&kernmem)]=pdpt_e;
  
  //creating pdt to map to pdpt
  uint64_t pdt_e = (uint64_t)pdt;
  pdt_e|=PRESENT;
  pdt_e|=WRITEABLE;
  pdt_e|=USER;
  pdpt->entries[get_PDPT_INDEX((uint64_t)&kernmem)]=pdt_e;
  
  //creating pdt to map to pdpt
  uint64_t pt_e = (uint64_t)pt;
  pt_e|=PRESENT;
  pt_e|=WRITEABLE;
  pt_e|=USER;
  pdt->entries[get_PDT_INDEX((uint64_t)&kernmem)]=pt_e;
  
  physfree+=0x5000; //TODO: no idea why additonal memory is added.. need to verify
  uint64_t v_addr=(uint64_t)&kernmem;
  while(physbase<physfree){
    uint64_t entry=physbase;
    entry|=PRESENT;
    entry|=WRITEABLE;
    entry|=USER;
    pt->entries[get_PT_INDEX(v_addr)]=entry;
    physbase+=0x1000;
    v_addr+=0x1000;
  }
  
  vga_virtual_address=v_addr;
  vga_end_virtual_address = v_addr+0x3000;
  mapPage(vga_virtual_address,VGA_ADDRESS);
  mapVGA(vga_virtual_address);

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
    pml_entry = (uint64_t)pdpt;
    pml_entry|=PRESENT;
    pml_entry|=WRITEABLE;
    pml_entry|=USER;
    pml4->entries[get_PML4_INDEX((uint64_t)v_addr)]=pml_entry;
  }
  
  uint64_t pdpt_entry = pdpt->entries[get_PDPT_INDEX((uint64_t)v_addr)];
  if(pdpt_entry&PRESENT){
    pdt = (struct PDT*)(pdpt_entry&FRAME);
  }
  else{
    pdt = (struct PDT*)pageAllocator();
    pdpt_entry = (uint64_t)pdt;
    pdpt_entry|=PRESENT;
    pdpt_entry|=WRITEABLE;
    pdpt_entry|=USER;
    pdpt->entries[get_PDPT_INDEX((uint64_t)v_addr)]=pdpt_entry;
  }
  
  uint64_t pdt_entry = pdt->entries[get_PDT_INDEX((uint64_t)v_addr)];
  if(pdt_entry&PRESENT){
    pt = (struct PT*)(pdt_entry&FRAME);
  }
  else{
    pt = (struct PT*)pageAllocator();
    pdt_entry = (uint64_t)pt;
    pdt_entry|=PRESENT;
    pdt_entry|=WRITEABLE;
    pdt_entry|=USER;
    pdt->entries[get_PDT_INDEX((uint64_t)v_addr)]=pdt_entry;
  }
  
  uint64_t pt_entry = phy_addr;
  pt_entry|=PRESENT;
  pt_entry|=WRITEABLE;
  pt_entry|=USER;
  pt->entries[get_PT_INDEX((uint64_t)v_addr)]=pt_entry;
  
}

void enablePaging(){
  __asm__ __volatile__("mov %0,%%cr3":: "b"((uint64_t)pml4));
  /*uint64_t temp=0;
  __asm__ __volatile__("mov %%cr0,%0":"=a"(temp):);
  temp|=1<<31;
  kprintf("CR0:- %p",temp);
  __asm__ __volatile__("mov %0,%%cr0":: "b"((uint64_t)temp));*/
  
}



