#ifndef _GDT_H
#define _GDT_H

void init_gdt(void* initialStackPtr);
void set_tss_rsp(void *rsp);

#endif
