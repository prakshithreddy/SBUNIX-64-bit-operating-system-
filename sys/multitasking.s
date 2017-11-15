.section .text

.global _switchThread_,_switchToRingThree

_switchThread_:
	movq %rax,(%rdi)
	popq %rax
	movq %rax,64(%rdi)
	movq %rbx,8(%rdi)
	movq %rcx,16(%rdi)
	movq %rdx,24(%rdi)
	movq %rsi,32(%rdi)
	movq %rdi,40(%rdi)
	movq %rbp,56(%rdi)
	/*movq %rip,64(%rdi)*/
	pushfq
	pop %rax
	movq %rax,72(%rdi)
	movq %cr3,%rax
	movq %rax,80(%rdi)
	movq %rsp,48(%rdi)  /*storing all the registers at the address pointed by rdi*/
	
	movq 72(%rsi),%rax
    pushq %rax
	popfq
	movq 8(%rsi),%rbx
	movq 16(%rsi),%rcx
	movq 24(%rsi),%rdx
	/*movq 32(%rsi),%rsi*/
	movq 40(%rsi),%rdi
	movq 56(%rsi),%rbp
	
	movq 48(%rsi),%rsp
	
	mov 80(%rsi),%rax
	mov %rax,%cr3
	
	mov 64(%rsi),%rax
	push %rax
	
	/*movq 64(%rsi),%rip*/
	
	movq (%rsi),%rax	/*reloading the registers with other tasks left over register values*/
	ret

_switchToRingThree:
    movq %rax,(%rdi)
    popq %rax
    movq %rax,64(%rdi)
    movq %rbx,8(%rdi)
    movq %rcx,16(%rdi)
    movq %rdx,24(%rdi)
    movq %rsi,32(%rdi)
    movq %rdi,40(%rdi)
    movq %rbp,56(%rdi)
    /*movq %rip,64(%rdi)*/
    pushfq
    popq %rax
    movq %rax,72(%rdi)
    movq %cr3,%rax
    movq %rax,80(%rdi)
    movq %rsp,48(%rdi)  /*storing all the registers at the address pointed by rdi*/

    cli

    movq 80(%rsi),%rax //loading the user cr3 val
    movq %rax, %cr3

    movq $0x23,%rax
    mov %rax,%ds
    mov %rax,%es   //setting segment registers
    movq %rax,%fs
    mov %rax,%gs


    movq 48(%rsi),%rax
    movq %rax,%rsp
    pushq $0x23
    pushq %rax
    movq 72(%rsi),%rax
    pushq %rax
    popq %rax
    orq $0x200,%rax
    pushq %rax
    pushq $0x2B
    movq 64(%rsi),%rax
    pushq %rax
    movq 8(%rsi),%rbx
    movq 16(%rsi),%rcx
    movq 24(%rsi),%rdx
    /*movq 32(%rsi),%rsi*/
    movq 40(%rsi),%rdi
    movq 56(%rsi),%rbp
    movq (%rsi),%rax
    iretq
