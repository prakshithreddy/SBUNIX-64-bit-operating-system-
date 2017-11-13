#include<sys/usermode.h>
#include<sys/gdt.h>


void switchToUserMode();

void initUserMode()
{
    switchToUserMode();
}
