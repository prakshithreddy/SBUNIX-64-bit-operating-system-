//Command source,dest
// at & t sytax ?? Intel sytax is opposite;
.text

//exposes it as a entity visible to other functions

.globl syscall5

syscall5:
movq %rdi, %rax
movq %rsi, %rdi
movq %rdx, %rsi
movq %rcx, %rdx
movq %r8, %r10
movq %r9, %r8
syscall
retq


