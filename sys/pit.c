#include<sys/pit.h>
#include<sys/defs.h>
#include<sys/pic.h>


void init_pit()
{
    outb(0x43, 0x36);
    outb(0x40, 0);
    outb(0x40, 0);
    
}
