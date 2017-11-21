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
    pushq r11
    movq %r10,%rcx
    pushq %rbx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15
    pushq %rbp
    pushq %rax
    call syscallHandler
    addq $0x8,%rsp
    popq %rbp
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %r10
    popq %r9
    popq %r8
    popq %rdi
    popq %rsi
    popq %rdx
    popq %rbx
    popq %rcx
    popq %r11
    popq %rsp
    sysretq
