.text

.global _switchToUserMode
.extern _inUserMode

_switchToUserMode:
    cli
    mov $0x23,%rax
    movq %rax, %es
    movq %rax, %fs
    movq %rax, %gs
    movq %rax, %ds
    movq %rax,%rax
    pushq $0x23
    pushq %rsp
    pushfq
//    popq %rax
//    or $0x200,%rax
//    pushq %rax
    pushq $0x1B
    //pushq _inUserMode
    lea [a],%rax
    pushq %rax
//sti
    iretq
a:
    add $8,%rsp

