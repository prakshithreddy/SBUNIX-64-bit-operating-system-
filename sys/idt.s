.text


.global _key_board_intr,_timer_intr,_generic_intr,_rtc_intr,_common_interrupt
.extern _key_press_handler,_timer_intr_hdlr,_rtc_intr_hndlr,_common_interrupt_hndlr,_generic_intr_hndlr


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

_generic_intr:

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

    call _generic_intr_hndlr

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


