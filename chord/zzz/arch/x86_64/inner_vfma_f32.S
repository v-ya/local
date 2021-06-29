.file	"inner_vfma_f32.S"

.text

// void chord_zzz_arch_inner_vfma_f32(float *restrict r, const float *restrict s, const float *restrict k, uintptr_t n)
// rdi: r
// rsi: s(32bytes align)
// rdx: k(32bytes align)
// rcx: n
	.p2align 4
	.globl	chord_zzz_arch_inner_vfma_f32
	.hidden	chord_zzz_arch_inner_vfma_f32
	.type	chord_zzz_arch_inner_vfma_f32, @function
chord_zzz_arch_inner_vfma_f32:
	movq	%rcx, %r8
	andq	$7, %r8
	shrq	$3, %rcx
	xorl	%eax, %eax
	jmp	.L_loop_check
	.p2align 4,,10
	.p2align 4
	.L_loop:
	vmovaps	(%rsi, %rax), %ymm0
	vmovaps	(%rdx, %rax), %ymm1
	vmulps	%ymm1, %ymm0, %ymm0
	vmovups	(%rdi, %rax), %ymm1
	vaddps	%ymm1, %ymm0, %ymm0
	vmovups	%ymm0, (%rdi, %rax)
	leaq	32(%rax), %rax
	subq	$1, %rcx
	.L_loop_check:
	testq	%rcx, %rcx
	jne	.L_loop
	jmp	.L_loop_rem_check
	.p2align 4,,10
	.p2align 4
	.L_loop_rem:
	vmovss	(%rsi, %rax), %xmm0
	vmovss	(%rdx, %rax), %xmm1
	vmulss	%xmm1, %xmm0, %xmm0
	vmovss	(%rdi, %rax), %xmm1
	vaddss	%xmm1, %xmm0, %xmm0
	vmovss	%xmm0, (%rdi, %rax)
	leaq	4(%rax), %rax
	subq	$1, %r8
	.L_loop_rem_check:
	testq	%r8, %r8
	jne	.L_loop_rem
	vzeroupper
	retq
	.size	chord_zzz_arch_inner_vfma_f32, .-chord_zzz_arch_inner_vfma_f32
