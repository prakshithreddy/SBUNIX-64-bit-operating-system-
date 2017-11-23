#ifndef _PCI_H
#define _PCI_H
#include<sys/defs.h>
#include<sys/kprintf.h>

uint32_t readPIC(uint16_t bus,uint16_t device,uint16_t function,uint32_t offset);
void writePIC(uint16_t bus,uint16_t device,uint16_t function,uint32_t offset,uint32_t value);
int ifMultiFunction(uint16_t bus,uint16_t device);
void printALLDrivers();
void probeAHCI();
#endif
