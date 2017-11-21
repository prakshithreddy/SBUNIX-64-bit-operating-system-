#ifndef _WAIT_H
#define _WAIT_H

#include<sys/defs.h>

pid_t wait(int *status);

int wait4(pid_t id, int *status,int options,void* rusage);

int waitpid(int pid, int *status,int options);

#endif

