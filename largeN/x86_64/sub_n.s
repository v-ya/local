	.file	"sub_n.s"
	.text
	.p2align 4
	.globl	largeN_sub_n
	.type	largeN_sub_n, @function

// (largeN_t *r:%rdi, uint64_t v:%rsi, uint64_t i:%rdx)
largeN_sub_n:
.LFB0:
	.cfi_startproc
	movq (%rdi), %rcx
	movq %rdi, %rax
	cmpq %rcx, %rdx
	jae .L_return
	lea 8(%rdi), %rdi
	xorl %r9d, %r9d
	.p2align 4,,10
	.p2align 3
.L_loop:
	xorl %r8d, %r8d
	subq %rsi, (%rdi, %rdx, 8)
	jnc .L_return
	adcq %r9, %r8
	addq $1, %rdx
	cmpq %rcx, %rdx
	jae .L_return
	xorl %esi, %esi
	subq %r8, (%rdi, %rdx, 8)
	jnc .L_return
	adcq %r9, %rsi
	addq $1, %rdx
	cmpq %rcx, %rdx
	jb .L_loop
.L_return:
	ret
	.cfi_endproc

.LFE0:
	.size	largeN_sub_n, .-largeN_sub_n
