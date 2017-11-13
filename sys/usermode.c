#include<sys/usermode.h>
#include<sys/gdt.c>


void switchToUserMode();

void initUserMode()
{
    switchToUserMode();
}
