#include <sys/defs.h>
#include <sys/idt.h>
#include <sys/pic.h>
#include <sys/kprintf.h>

struct idt_descriptor {
    uint16_t id_base_lo;
    uint16_t id_sel;
    uint8_t id_always0;
    uint8_t id_flags;
    uint16_t id_base_middle;
    uint32_t id_base_high;
    uint32_t id_always00;
}__attribute__((packed));


struct idtr_t {
    uint16_t limit;
    uint64_t base;
}__attribute__((packed));


struct idt_descriptor idt[256];
static const struct idtr_t idtp = {(sizeof(struct idt_descriptor)*256)-1,(uint64_t)idt};

void id_set_gate(uint8_t intr_num,uint64_t base_addr, uint8_t sel,uint8_t flags)
{
    idt[intr_num].id_flags = flags;
    idt[intr_num].id_always0 = 0;
    idt[intr_num].id_always00 = 0;
    idt[intr_num].id_sel = sel;
    
    //ADDRESS TO THE INTERRUPT SERVICE ROUTINE METHOD
    
    idt[intr_num].id_base_lo = (base_addr & 0xFFFF);
    idt[intr_num].id_base_middle = (base_addr >> 16) & 0xFFFF;
    idt[intr_num].id_base_high = (base_addr >> 32) & 0xFFFFFFFF;
    
}

void _key_press_handler(){
    
    uint8_t a;
    
    a = inb(0x60);
    
    kprintf("%c",a);
    
    
   // kprintf("hi");
    outb(0x20,0x20);
    outb(0x20,0xA0);
}

static int i = 0;
static int j =0;

void _timer_intr_hdlr(){
    
    i++;
    
    if(i%19==0) { i = 0; kprintf("%d ",j); j++;}
    
    
    // kprintf("hi");
    outb(0x20,0x20);
    outb(0x20,0xA0);
}


void _key_board_intr();


void _timer_intr();

void init_idt()
{
    //for(int i=0;i<256;i++)
    id_set_gate(33,(uint64_t)_key_board_intr,8,0x8E);
     id_set_gate(32,(uint64_t)_timer_intr,8,0x8E);
    //kprintf("Loading IDT prashanth 123123123\n");
    __asm__ __volatile__("lidt %0" : : "m" (idtp));
    
}
