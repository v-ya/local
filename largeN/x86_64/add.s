	.file	"add.s"
	.text
	.p2align 4
	.globl	largeN_add
	.type	largeN_add, @function

// (largeN_t *r:%rdi, const largeN_t *a:%rsi)
largeN_add:
.LFB0:
	.cfi_startproc
	movq (%rdi), %rcx
	movq (%rsi), %rdx
	cmpq %rcx, %rdx
	cmovb %rdx, %rcx
	lea 8(%rdi), %rdi
	lea 8(%rsi), %rsi
	// zero:%r9
	xorl %r9d, %r9d
	// uint64_t i:%rdx, n:%rcx
	xorl %edx, %edx
	xorl %eax, %eax
	cmpq %rcx, %rdx
	jae .L_return
	.p2align 4,,10
	.p2align 3
.L_loop:
	// r + a => %rax => r
	xorl %r8d, %r8d
	addq (%rdi, %rdx, 8), %rax
	adcq %r9, %r8
	addq (%rsi, %rdx, 8), %rax
	adcq %r9, %r8
	movq %rax, (%rdi, %rdx, 8)
	addq $1, %rdx
	cmpq %rcx, %rdx
	jae .L_break
	// r + a => %r8 => r
	xorl %eax, %eax
	addq (%rdi, %rdx, 8), %r8
	adcq %r9, %rax
	addq (%rsi, %rdx, 8), %r8
	adcq %r9, %rax
	movq %r8, (%rdi, %rdx, 8)
	addq $1, %rdx
	cmpq %rcx, %rdx
	jb .L_loop
	movq %rax, %r8
.L_break:
	testq %r8, %r8
	jz .L_return
	cmpq -8(%rdi), %rcx
	jae .L_return
	lea -8(%rdi), %rdi
	movq %r8, %rsi
	jmp largeN_add_n
.L_return:
	lea -8(%rdi), %rax
	ret
	.cfi_endproc

.LFE0:
	.size	largeN_add, .-largeN_add
