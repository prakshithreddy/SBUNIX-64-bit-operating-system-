.text

.global userRSP
.global kernelRSP

.global _syscallEntry

.global syscallHandler

_syscallEntry:
    movq %rsp, userRSP
    movq (kernelRSP),%rsp
    pushq (userRSP)
    pushq %r11
    pushq %rdx
    pushq %rbx
    pushq %rsi
    pushq %rdi
    pushq %r13
    pushq %r8
    pushq %r9
    pushq %r12
    pushq %r10
    pushq %r15
    pushq %r14
    pushq %rbp
    pushq %rcx
    pushq %rax
    movq %r10,%rcx
    call syscallHandler
    addq $0x8,%rsp // rax contains the returnValue, so not popping
    pushq %rcx
    popq %rbp
    popq %r14
    popq %r15
    popq %r10
    popq %r12
    popq %r9
    popq %r8
    popq %r13
    popq %rdi
    popq %rsi
    popq %rbx
    popq %rdx
    popq %r11
    popq %rsp
    sysretq
