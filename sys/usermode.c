#include<sys/usermode.h>
#include<sys/defs.h>
#include<sys/kprintf.h>


void switchToUserMode(uint64_t userModeDataSel);

void initUserMode()
{
    switchToUserMode((GDT_DS|P|W|DPL3));
}
