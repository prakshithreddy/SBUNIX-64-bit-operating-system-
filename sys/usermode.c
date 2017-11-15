#include<sys/usermode.h>
#include<sys/defs.h>
#include<sys/kprintf.h>

#define GDT_CS        (0x00180000000000)  /*** code segment descriptor ***/
#define GDT_DS        (0x00100000000000)  /*** data segment descriptor ***/

#define C             (0x00040000000000)  /*** conforming ***/
#define DPL0          (0x00000000000000)  /*** descriptor privilege level 0 ***/
#define DPL1          (0x00200000000000)  /*** descriptor privilege level 1 ***/
#define DPL2          (0x00400000000000)  /*** descriptor privilege level 2 ***/
#define DPL3          (0x00600000000000)  /*** descriptor privilege level 3 ***/
#define P             (0x00800000000000)  /*** present ***/
#define L             (0x20000000000000)  /*** long mode ***/
#define D             (0x40000000000000)  /*** default op size ***/
#define W             (0x00020000000000)  /*** writable data segment ***/


void _inUserMode()
{
    int i=0;
    i+=1;
    //kprintf("INUSERMOD
}

void _switchToUserMode(uint64_t userModeDataSel,uint64_t userModeCodeSel);

void initUserMode()
{
    //kprintf("%p ",(GDT_DS|P|W|DPL3));
    _switchToUserMode();
}


