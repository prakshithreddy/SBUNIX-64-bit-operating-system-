.text



//0    Division By Zero Exception    No
//1    Debug Exception    No
//2    Non Maskable Interrupt Exception    No
//3    Breakpoint Exception    No
//4    Into Detected Overflow Exception    No
//5    Out of Bounds Exception    No
//6    Invalid Opcode Exception    No
//7    No Coprocessor Exception    No
//8    Double Fault Exception    Yes
//9    Coprocessor Segment Overrun Exception    No
//10    Bad TSS Exception    Yes
//11    Segment Not Present Exception    Yes
//12    Stack Fault Exception    Yes
//13    General Protection Fault Exception    Yes
//14    Page Fault Exception    Yes
//15    Unknown Interrupt Exception    No
//16    Coprocessor Fault Exception    No
//17    Alignment Check Exception (486+)    No
//18    Machine Check Exception (Pentium/586+)    No
//19 to 31    Reserved Exceptions    No


.global _key_board_intr,_timer_intr,_rtc_intr,_common_interrupt,_isr0,_isr1,_isr2,_isr3,_isr4,_isr5,_isr6,_isr7,_isr8,_isr9,_isr10,_isr11,_isr12,_isr13,_isr14,_isr15,_isr16,_isr17,_isr18,_isr19,_isr20,_isr21,
.extern _key_press_handler,_timer_intr_hdlr,_rtc_intr_hndlr,_common_interrupt_hndlr,_hndlr_isr0,_hndlr_isr1,_hndlr_isr2,_hndlr_isr3,_hndlr_isr4,_hndlr_isr5,_hndlr_isr6,_hndlr_isr7,_hndlr_isr8,_hndlr_isr9,_hndlr_isr10,_hndlr_isr11,_hndlr_isr12,_hndlr_isr13,_hndlr_isr14,_hndlr_isr15,_hndlr_isr16,_hndlr_isr17,_hndlr_isr18,_hndlr_isr19,_hndlr_isr20,_hndlr_isr21

.global switchRsp
.global switchRax
.global switchRbx
.global switchRcx
.global switchRdx
.global switchRsi
.global switchRdi
.global switchRbp


_key_board_intr:
    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp


    pushq %fs
    pushq %gs
    pushq %rsp

    call _key_press_handler

    popq %rsp

    popq %gs
    popq %fs


    popq  %rbp
    popq  %rdi
    popq  %rsi
    popq  %rdx
    popq  %rcx
    popq  %rbx
    popq  %rax

    iretq

_timer_intr:

    movq %rsp,switchRsp

    movq %rax, switchRax
    movq %rbx, switchRbx
    movq %rcx, switchRcx
    movq %rdx, switchRdx
    movq %rsi, switchRsi
    movq %rdi, switchRdi
    movq %rbp, switchRbp

    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp


    pushq %fs
    pushq %gs
    pushq %rsp

    call _timer_intr_hdlr

    popq %rsp

    popq %gs
    popq %fs


    popq  %rbp
    popq  %rdi
    popq  %rsi
    popq  %rdx
    popq  %rcx
    popq  %rbx
    popq  %rax

    iretq

_isr0:

    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp


    pushq %fs
    pushq %gs
    pushq %rsp

    call _hndlr_isr0

    popq %rsp

    popq %gs
    popq %fs


    popq  %rbp
    popq  %rdi
    popq  %rsi
    popq  %rdx
    popq  %rcx
    popq  %rbx
    popq  %rax

_isr1:

    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp


    pushq %fs
    pushq %gs
    pushq %rsp

    call _hndlr_isr1

    popq %rsp

    popq %gs
    popq %fs


    popq  %rbp
    popq  %rdi
    popq  %rsi
    popq  %rdx
    popq  %rcx
    popq  %rbx
    popq  %rax

_isr2:

    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp


    pushq %fs
    pushq %gs
    pushq %rsp

    call _hndlr_isr2

    popq %rsp

    popq %gs
    popq %fs


    popq  %rbp
    popq  %rdi
    popq  %rsi
    popq  %rdx
    popq  %rcx
    popq  %rbx
    popq  %rax


_isr14:

    movq (%rsp),%rdi
    movq %rsp,%rax
    addq $16,%rax
    movq %rax,%rsi
    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp


    pushq %fs
    pushq %gs
    pushq %rsp

    call _hndlr_isr14

    popq %rsp

    popq %gs
    popq %fs


    popq  %rbp
    popq  %rdi
    popq  %rsi
    popq  %rdx
    popq  %rcx
    popq  %rbx
    popq  %rax


_isr21:

    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp


    pushq %fs
    pushq %gs
    pushq %rsp

    call _hndlr_isr21

    popq %rsp

    popq %gs
    popq %fs


    popq  %rbp
    popq  %rdi
    popq  %rsi
    popq  %rdx
    popq  %rcx
    popq  %rbx
    popq  %rax

_isr20:

    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp


    pushq %fs
    pushq %gs
    pushq %rsp

    call _hndlr_isr20

    popq %rsp

    popq %gs
    popq %fs


    popq  %rbp
    popq  %rdi
    popq  %rsi
    popq  %rdx
    popq  %rcx
    popq  %rbx
    popq  %rax

_isr19:

    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp


    pushq %fs
    pushq %gs
    pushq %rsp

    call _hndlr_isr19

    popq %rsp

    popq %gs
    popq %fs


    popq  %rbp
    popq  %rdi
    popq  %rsi
    popq  %rdx
    popq  %rcx
    popq  %rbx
    popq  %rax

_isr18:

    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp


    pushq %fs
    pushq %gs
    pushq %rsp

    call _hndlr_isr18

    popq %rsp

    popq %gs
    popq %fs


    popq  %rbp
    popq  %rdi
    popq  %rsi
    popq  %rdx
    popq  %rcx
    popq  %rbx
    popq  %rax

_isr17:

    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp


    pushq %fs
    pushq %gs
    pushq %rsp

    call _hndlr_isr17

    popq %rsp

    popq %gs
    popq %fs


    popq  %rbp
    popq  %rdi
    popq  %rsi
    popq  %rdx
    popq  %rcx
    popq  %rbx
    popq  %rax

_isr16:

    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp


    pushq %fs
    pushq %gs
    pushq %rsp

    call _hndlr_isr16

    popq %rsp

    popq %gs
    popq %fs


    popq  %rbp
    popq  %rdi
    popq  %rsi
    popq  %rdx
    popq  %rcx
    popq  %rbx
    popq  %rax

_isr15:

    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp


    pushq %fs
    pushq %gs
    pushq %rsp

    call _hndlr_isr15

    popq %rsp

    popq %gs
    popq %fs


    popq  %rbp
    popq  %rdi
    popq  %rsi
    popq  %rdx
    popq  %rcx
    popq  %rbx
    popq  %rax

_isr13:

    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp


    pushq %fs
    pushq %gs
    pushq %rsp

    call _hndlr_isr13

    popq %rsp

    popq %gs
    popq %fs


    popq  %rbp
    popq  %rdi
    popq  %rsi
    popq  %rdx
    popq  %rcx
    popq  %rbx
    popq  %rax


_isr12:

    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp


    pushq %fs
    pushq %gs
    pushq %rsp

    call _hndlr_isr12

    popq %rsp

    popq %gs
    popq %fs


    popq  %rbp
    popq  %rdi
    popq  %rsi
    popq  %rdx
    popq  %rcx
    popq  %rbx
    popq  %rax

_isr11:

    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp


    pushq %fs
    pushq %gs
    pushq %rsp

    call _hndlr_isr11

    popq %rsp

    popq %gs
    popq %fs


    popq  %rbp
    popq  %rdi
    popq  %rsi
    popq  %rdx
    popq  %rcx
    popq  %rbx
    popq  %rax

_isr10:

    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp


    pushq %fs
    pushq %gs
    pushq %rsp

    call _hndlr_isr10

    popq %rsp

    popq %gs
    popq %fs


    popq  %rbp
    popq  %rdi
    popq  %rsi
    popq  %rdx
    popq  %rcx
    popq  %rbx
    popq  %rax

_isr9:

    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp


    pushq %fs
    pushq %gs
    pushq %rsp

    call _hndlr_isr9

    popq %rsp

    popq %gs
    popq %fs


    popq  %rbp
    popq  %rdi
    popq  %rsi
    popq  %rdx
    popq  %rcx
    popq  %rbx
    popq  %rax

_isr8:

    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp


    pushq %fs
    pushq %gs
    pushq %rsp

    call _hndlr_isr8

    popq %rsp

    popq %gs
    popq %fs


    popq  %rbp
    popq  %rdi
    popq  %rsi
    popq  %rdx
    popq  %rcx
    popq  %rbx
    popq  %rax

_isr7:

    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp


    pushq %fs
    pushq %gs
    pushq %rsp

    call _hndlr_isr7

    popq %rsp

    popq %gs
    popq %fs


    popq  %rbp
    popq  %rdi
    popq  %rsi
    popq  %rdx
    popq  %rcx
    popq  %rbx
    popq  %rax

_isr6:

    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp


    pushq %fs
    pushq %gs
    pushq %rsp

    call _hndlr_isr6

    popq %rsp

    popq %gs
    popq %fs


    popq  %rbp
    popq  %rdi
    popq  %rsi
    popq  %rdx
    popq  %rcx
    popq  %rbx
    popq  %rax
    _isr5:

    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp


    pushq %fs
    pushq %gs
    pushq %rsp

    call _hndlr_isr5

    popq %rsp

    popq %gs
    popq %fs


    popq  %rbp
    popq  %rdi
    popq  %rsi
    popq  %rdx
    popq  %rcx
    popq  %rbx
    popq  %rax

_isr4:

    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp


    pushq %fs
    pushq %gs
    pushq %rsp

    call _hndlr_isr4

    popq %rsp

    popq %gs
    popq %fs


    popq  %rbp
    popq  %rdi
    popq  %rsi
    popq  %rdx
    popq  %rcx
    popq  %rbx
    popq  %rax

_isr3:

    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp


    pushq %fs
    pushq %gs
    pushq %rsp

    call _hndlr_isr3

    popq %rsp

    popq %gs
    popq %fs


    popq  %rbp
    popq  %rdi
    popq  %rsi
    popq  %rdx
    popq  %rcx
    popq  %rbx
    popq  %rax




_rtc_intr:

    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp


    pushq %fs
    pushq %gs
    pushq %rsp

    call _rtc_intr_hndlr

    popq %rsp

    popq %gs
    popq %fs


    popq  %rbp
    popq  %rdi
    popq  %rsi
    popq  %rdx
    popq  %rcx
    popq  %rbx
    popq  %rax

    iretq


