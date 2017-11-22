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
    movq %r10,%rcx
    pushq %rdx
    pushq %rbx
    pushq %rdi
    pushq %rsi

    pushq %r10
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15
    pushq %rbp
    pushq %r8
    pushq %r9
    pushq %rax
    call syscallHandler
    addq $0x8,%rsp // rax contains the returnValue, so not popping

    popq %r9
    popq %r8
    popq %rbp
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %r10


    popq %rsi
    popq %rdi
    popq %rbx
    popq %rdx

    popq %r11
    popq %rcx
    popq %rsp
    sysretq
