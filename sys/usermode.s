.text

.global _switchToUserMode
.extern _inUserMode

_switchToUserMode:
    cli
    movq %rdi, %es
    movq %rdi, %fs
    movq %rdi, %gs
    movq %rdi, %ds
    movq %rsp,%rax
    pushq %rdi
    pushq %rax
    pushfq
//    popq %rax
//    or %rax,0x200
//    pushq %rax ;
    pushq %rsi
    pushq _inUserMode
//sti
    iretq
