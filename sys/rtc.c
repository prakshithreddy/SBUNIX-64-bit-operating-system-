#include <sys/rtc.h>
#include <sys/pic.h>

void init_rtc()
{
    __asm__ __volatile__("cli":::);
    outb(0x8B,0x70);
    unsigned char prev=inb(0x71);
    outb(0x8B,0x70);
    outportb(0x71, prev | 0x40);
    __asm__ __volatile__("sti":::);
    
}
