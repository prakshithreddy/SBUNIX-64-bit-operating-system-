#ifndef _SYSCALL_H
#define _SYSCALL_H

#include<sys/defs.h>

extern uint64_t userRSP;
extern uint64_t kernelRSP;


void initSyscalls();

uint64_t readMSR(uint32_t msrId);
void writeMSR(unint64_t value,uint32_t msrId);

int64_t syscall(int64_t syscallNum,int64_t param1,int64_t param2,int64_t param3,int64_t param4,int64_t param5,int64_t param6);

#endif
