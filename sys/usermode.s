
DSIDX equ 0x23

.text



.global _switchToUserMode
.extern _inUserMode

_switchToUserMode:
  cli
    movq DSIDX, %es
    movq DSIDX, %fs
    movq DSIDX, %gs
    movq DSIDX, %ds
    movq %rsp,%rax
    pushq DSIDX
    pushq %rax
    pushfq
    popq %rax
    or %rax,0x200
    pushq %rax
    pushq %rsi
    pushq _inUserMode
//sti
    iretq
