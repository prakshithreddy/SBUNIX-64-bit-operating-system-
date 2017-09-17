.text


.global _key_board_intr,_timer_intr,_rtc_intr
.extern _key_press_handler,_timer_intr_hdlr,_rtc_intr_hndlr


_key_board_intr:
    push %rax
    push %rbx
    push %rcx
    push %rdx
    push %rsi
    push %rdi
    push %rbp


    pushq %fs
    pushq %gs
    pushq %rsp

    call _key_press_handler

    pop %rsp

    popq %gs
    popq %fs


    pop  %rbp
    pop  %rdi
    pop  %rsi
    pop  %rdx
    pop  %rcx
    pop  %rbx
    pop  %rax

    iretq

_timer_intr:

    push %rax
    push %rbx
    push %rcx
    push %rdx
    push %rsi
    push %rdi
    push %rbp


    pushq %fs
    pushq %gs
    pushq %rsp

    call _timer_intr_hdlr

    pop %rsp

    popq %gs
    popq %fs


    pop  %rbp
    pop  %rdi
    pop  %rsi
    pop  %rdx
    pop  %rcx
    pop  %rbx
    pop  %rax

    iretq

_rtc_intr:

push %rax
push %rbx
push %rcx
push %rdx
push %rsi
push %rdi
push %rbp


pushq %fs
pushq %gs
pushq %rsp

call _timer_intr_hdlr

pop %rsp

popq %gs
popq %fs


pop  %rbp
pop  %rdi
pop  %rsi
pop  %rdx
pop  %rcx
pop  %rbx
pop  %rax

iretq

