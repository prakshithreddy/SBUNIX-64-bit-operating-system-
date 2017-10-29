#include<sys/virtualMemory.h>
#include<sys/PhyMemMapper.h>

void mapKernelMemory(){
  uint64_t physbase = KER_PHYSBASE;
  uint64_t physfree = KER_PHYSFREE;
  pml4 = (struct PML4*)pageAllocator();
  struct PDPT *pdpt = (struct *PDPT)pageAllocator();
  struct PDT *pdt = (struct *PDT)pageAllocator();
  struct PT *pt = (struct *PT)pageAllocator();
  
  //creating pdpt to map to pml4
  uint64_t pdpt_e = (uint64_t)pdpt;
  pdpt_e|=PRESENT;
  pdpt_e|=WRITEABLE;
  pdpt_e|=USER
  pml4->entries[get_PML4_INDEX((uint64_t)&kernmem)]=pdpt_e;
  
  //creating pdt to map to pdpt
  uint64_t pdt_e = (uint64_t)pdt;
  pdt_e|=PRESENT;
  pdt_e|=WRITEABLE;
  pdt_e|=USER
  pdpt->entries[get_PDPT_INDEX((uint64_t)&kernmem)]=pdt_e;
  
  //creating pdt to map to pdpt
  uint64_t pt_e = (uint64_t)pt;
  pt_e|=PRESENT;
  pt_e|=WRITEABLE;
  pt_e|=USER
  pdt->entries[get_PDT_INDEX((uint64_t)&kernmem)]=pt_e;
  
  phsfree+=0x4800; //TODO: no idea why additonal memory is added.. need to verify
  uint64_t v_addr=(uint64_t)&kernmem;
  while(physbase<physfree){
    uint64_t entry=physbase
    entry|=PRESENT;
    entry|=WRITEABLE;
    entry|=USER;
    pt->entries[get_PT_INDEX(v_addr)]=entry
    physbase+=0x1000;
    v_addr+=0x1000;
  }

}