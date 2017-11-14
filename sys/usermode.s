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
    movq %rsp,%rax
    pushq $0x23
    pushq %rax
    pushfq
    popq %rax
    or $0x200,%rax
    pushq %rax
    pushq $0x1B
    pushq $1f;
//sti
    iretq
1:
    
