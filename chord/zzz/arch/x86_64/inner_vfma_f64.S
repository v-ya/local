.file	"inner_vfma_f64.S"

.text

// void chord_zzz_arch_inner_vfma_f64(double *restrict r, const double *restrict s, const double *restrict k, uintptr_t n)
// rdi: r
// rsi: s(32bytes align)
// rdx: k(32bytes align)
// rcx: n
	.p2align 4
	.globl	chord_zzz_arch_inner_vfma_f64
	.hidden	chord_zzz_arch_inner_vfma_f64
	.type	chord_zzz_arch_inner_vfma_f64, @function
chord_zzz_arch_inner_vfma_f64:
	movq	%rcx, %r8
	andq	$3, %r8
	shrq	$2, %rcx
	xorl	%eax, %eax
	jmp	.L_loop_check
	.p2align 4,,10
	.p2align 4
	.L_loop:
	vmovapd	(%rsi, %rax), %ymm0
	vmovapd	(%rdx, %rax), %ymm1
	vmulpd	%ymm1, %ymm0, %ymm0
	vmovupd	(%rdi, %rax), %ymm1
	vaddpd	%ymm1, %ymm0, %ymm0
	vmovupd	%ymm0, (%rdi, %rax)
	leaq	32(%rax), %rax
	subq	$1, %rcx
	.L_loop_check:
	testq	%rcx, %rcx
	jne	.L_loop
	jmp	.L_loop_rem_check
	.p2align 4,,10
	.p2align 4
	.L_loop_rem:
	vmovsd	(%rsi, %rax), %xmm0
	vmovsd	(%rdx, %rax), %xmm1
	vmulsd	%xmm1, %xmm0, %xmm0
	vmovsd	(%rdi, %rax), %xmm1
	vaddsd	%xmm1, %xmm0, %xmm0
	vmovsd	%xmm0, (%rdi, %rax)
	leaq	8(%rax), %rax
	subq	$1, %r8
	.L_loop_rem_check:
	testq	%r8, %r8
	jne	.L_loop_rem
	vzeroupper
	retq
	.size	chord_zzz_arch_inner_vfma_f64, .-chord_zzz_arch_inner_vfma_f64
