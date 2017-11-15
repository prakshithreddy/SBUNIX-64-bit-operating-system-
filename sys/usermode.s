.text

.global _switchToRingThree

_switchToRingThree:
    cli
    mov $0x23,%rax
    movq %rax, %es
    movq %rax, %fs
    movq %rax, %gs
    movq %rax, %ds
    pushq $0x23
    pushq %rsp
    pushfq
    pushq $0x2B
    pushq $1f
//sti
    iret
1:
    add $8,%rsp

