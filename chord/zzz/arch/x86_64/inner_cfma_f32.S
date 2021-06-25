.file	"inner_cfma_f32.S"

.text

// void chord_zzz_arch_inner_cfma_f32(float *restrict r, const float *restrict s, uintptr_t n, float k)
// rdi: r
// rsi: s(32bytes align)
// rdx: n
// xmm0: k
	.p2align 4
	.globl	chord_zzz_arch_inner_cfma_f32
	.hidden	chord_zzz_arch_inner_cfma_f32
	.type	chord_zzz_arch_inner_cfma_f32, @function
chord_zzz_arch_inner_cfma_f32:
	vbroadcastss %xmm0, %ymm2
	xorl	%ecx, %ecx
	movq	%rdx, %rax
	andq	$7, %rax
	shrq	$3, %rdx
	jmp	.L_loop_check
	.p2align 4,,10
	.p2align 4
	.L_loop:
	vmovaps	(%rsi, %rcx), %ymm0
	vmovups	(%rdi, %rcx), %ymm1
	vmulps	%ymm2, %ymm0, %ymm0
	vaddps	%ymm1, %ymm0, %ymm0
	vmovups	%ymm0, (%rdi, %rcx)
	leaq	32(%rcx), %rcx
	subq	$1, %rdx
	.L_loop_check:
	testq	%rdx, %rdx
	jne	.L_loop
	jmp	.L_loop_rem_check
	.p2align 4,,10
	.p2align 4
	.L_loop_rem:
	movss	(%rsi, %rcx), %xmm0
	movss	(%rdi, %rcx), %xmm1
	mulss	%xmm2, %xmm0
	addss	%xmm1, %xmm0
	movss	%xmm0, (%rdi, %rcx)
	leaq	4(%rcx), %rcx
	subq	$1, %rax
	.L_loop_rem_check:
	testq	%rax, %rax
	jne	.L_loop_rem
	retq
	.size	chord_zzz_arch_inner_cfma_f32, .-chord_zzz_arch_inner_cfma_f32
