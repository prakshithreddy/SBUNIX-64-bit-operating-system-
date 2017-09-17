#include <sys/rtc.h>
#include <sys/pic.h>

void init_rtc()
{
    __asm__ __volatile__("cli":::);
    outb(0x8B,0x70);
    unsigned char prev=inb(0x71);
    outb(0x8B,0x70);
    outb(prev | 0x40,0x71);
    __asm__ __volatile__("sti":::);
    
}
