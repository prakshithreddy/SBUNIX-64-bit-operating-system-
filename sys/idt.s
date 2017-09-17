.text


.global _key_board_intr,_timer_intr
.extern _key_press_handler,_timer_intr_hdlr


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

    movl $0x07690748,0xb8000

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

    movl %rdi,0xb8f9e

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
