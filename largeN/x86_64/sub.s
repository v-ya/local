	.file	"sub.s"
	.text
	.p2align 4
	.globl	largeN_sub
	.type	largeN_sub, @function

// (largeN_t *r:%rdi, const largeN_t *a:%rsi)
largeN_sub:
.LFB0:
	.cfi_startproc
	movq (%rdi), %rcx
	movq (%rsi), %rdx
	cmpq %rcx, %rdx
	cmovb %rdx, %rcx
	lea 8(%rdi), %rdi
	lea 8(%rsi), %rsi
	// uint64_t i:%rdx, n:%rcx
	xorl %edx, %edx
	xorl %r9d, %r9d
	cmpq %rcx, %rdx
	jae .L_return
	.p2align 4,,10
	.p2align 3
.L_loop:
	// r + a => %rax => r
	xorl %r8d, %r8d
	movq (%rdi, %rdx, 8), %rax
	subq %r9, %rax
	adcq $0, %r8
	subq (%rsi, %rdx, 8), %rax
	adcq $0, %r8
	movq %rax, (%rdi, %rdx, 8)
	addq $1, %rdx
	cmpq %rcx, %rdx
	jae .L_break
	// r + a => %r8 => r
	xorl %r9d, %r9d
	movq (%rdi, %rdx, 8), %rax
	subq %r8, %rax
	adcq $0, %r9
	subq (%rsi, %rdx, 8), %rax
	adcq $0, %r9
	movq %rax, (%rdi, %rdx, 8)
	addq $1, %rdx
	cmpq %rcx, %rdx
	jb .L_loop
	movq %r9, %r8
.L_break:
	testq %r8, %r8
	jz .L_return
	cmpq -8(%rdi), %rcx
	jae .L_return
	lea -8(%rdi), %rdi
	movq %r8, %rsi
	jmp largeN_sub_n
.L_return:
	lea -8(%rdi), %rax
	ret
	.cfi_endproc

.LFE0:
	.size	largeN_sub, .-largeN_sub
