#include<sys/pic.h>
#include<sys/kprintf.h>
#include<sys/defs.h>


uint8_t inb(uint16_t port)
{
    uint8_t val;
    __asm__ __volatile__("inb %1,%0;":"=a" (val): "Nd" (port));
    return val;
}


void outb(uint8_t data,uint16_t port)
{
    __asm__ __volatile__("outb %0,%1;": :"a" (data),"Nd" (port));
}

void init_pic(){
    
    outb(0x11,0x20);
    outb(0x11,0xA0);
    
    outb(0x20,0x21);
    outb(0x28,0xA1);
    
    outb(0x04,0x21);
    outb(0x02,0xA1);
    
    outb(0x01,0x21);
    outb(0x01,0xA1);
    
    outb(0xF8,0x21);
    outb(0xFD,0xA1);
    
    
    __asm__ __volatile__ ("sti");

}
