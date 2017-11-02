#ifndef __KPRINTF_H
#define __KPRINTF_H

#include<sys/defs.h>
void kprintf(const char *fmt, ...);

void keypress_bar(char *s,int color);
void boot_time_bar(int hours,int minutes,int seconds,int color);
void time_bar(int hours,int minutes,int seconds,int color);
void mapVGA(uint64_t vga_virtual_adrs);

#endif
