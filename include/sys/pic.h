#ifndef _PIC_H
#define _PIC_H

#include<sys/defs.h>

void init_pic();

uint8_t inb(uint16_t port);

void outb(uint8_t data,uint16_t port);



#endif
