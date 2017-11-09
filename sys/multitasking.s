.section .text

.global _switchThread_

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