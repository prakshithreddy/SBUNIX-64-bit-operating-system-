.text


.global _div0
.extern _fault_handler


_div0:
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
