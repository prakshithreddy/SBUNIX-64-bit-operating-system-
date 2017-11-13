.text

.global switchToUserMode

switchToUserMode:
    cli
    movq %rdi, %es
    movq %rdi, %fs
    movq %rdi, %gs
    movq %rdi, %ds
    movq %rsp,%rax
    pushq %rdi
    pushq %rax
    pushfq
    popq %rax
    orq %rax,$0x200
    pushq %rax
    pushq %rsi
    pushq %rdx
    iret
