.text

.global userRSP
.global kernelRSP

.global _syscallEntry

.global syscallHandler

_syscallEntry:
    movq %rsp, userRSP
    movq (kernelRSP),%rsp
    pushq (userRSP)

    pushq %rcx
    pushq %r11

    pushq %rdx
    pushq %rbx
    pushq %rdi
    pushq %rsi





    pushq %r15
    pushq %r14
    pushq %r13
    pushq %r12
    pushq %r10

    pushq %rbp
    pushq %r8
    pushq %r9
    pushq %rax
    movq %r10,%rcx


    call syscallHandler

    addq $0x8,%rsp // rax contains the returnValue, so not popping

    popq %r9
    popq %r8
    popq %rbp

    // restore all general purpose registers

    popq %r10
    popq %r12
    popq %r13
    popq %r14
    popq %r15




    popq %rsi
    popq %rdi
    popq %rbx
    popq %rdx

    popq %r11
    popq %rcx
    popq %rsp

    sysretq
