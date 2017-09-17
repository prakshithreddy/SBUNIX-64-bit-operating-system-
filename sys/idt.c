#include <sys/defs.h>
#include <sys/idt.h>
#include <sys/pic.h>
#include <sys/keyboard.h>
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

static int shift=0,control=0;

void _key_press_handler(){
    
    unsigned char a;
    
    a = inb(0x60);
    
    //kprintf("\n%d\n",a);
    
    if(a<128) {
        
        switch(a)
        {
            case 42:
                shift=1;break;
            case 29:
                control = 1;break;
            case 28:
                keypress_bar("Enter",0x0F);break;
            case 77:
                keypress_bar("Right Arrow",0x0F);break;
            case 75:
                keypress_bar("Left Arrow",0x0F);break;
            case 72:
                keypress_bar("Up Arrow",0x0F);break;
            case 80:
                keypress_bar("Down Arrow",0x0F);break;
            case 57:
                keypress_bar("Space",0x0F);break;
            case 58:
                keypress_bar("Alt",0x0F);break;
            case 15:
                keypress_bar("Tab",0x0F);break;
            case 1:
                keypress_bar("Esc",0x0F);break;
            case 59:
                keypress_bar("F1",0x0F);break;
            case 60:
                keypress_bar("F2",0x0F);break;
            case 61:
                keypress_bar("F3",0x0F);break;
            case 62:
                keypress_bar("F4",0x0F);break;
            case 63:
                keypress_bar("F5",0x0F);break;
            case 64:
                keypress_bar("F6",0x0F);break;
            case 65:
                keypress_bar("F7",0x0F);break;
            case 66:
                keypress_bar("F8",0x0F);break;
            case 67:
                keypress_bar("F9",0x0F);break;
            case 68:
                keypress_bar("F10",0x0F);break;
            case 69:
                keypress_bar("F11",0x0F);break;
            case 70:
                keypress_bar("F12",0x0F);break;
                
            default:
                if (shift==1)
                {
                    shift = 0;
                    keypress_bar("shift+'%c'='%c'",kbdus[a],CAPS_kbdus[a],0x0F);
                }
                else if (control==1)
                {
                    control=0;
                    keypress_bar("ctrl^%c",kbdus[a],0x0F);
                    
                }
                else
                {
                    keypress_bar("%c",kbdus[a],0x0F);
                }
        }
    }
    
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
    id_set_gate(33,(uint64_t)_key_board_intr,8,0x8E);
    id_set_gate(32,(uint64_t)_timer_intr,8,0x8E);
    __asm__ __volatile__("lidt %0" : : "m" (idtp));
    
}
