.file	"inner_cfma_f64.S"

.text

// void chord_zzz_arch_inner_cfma_f64(double *restrict r, const double *restrict s, uintptr_t n, double k)
// rdi: r
// rsi: s(32bytes align)
// rdx: n
// xmm0: k
	.p2align 4
	.globl	chord_zzz_arch_inner_cfma_f64
	.hidden	chord_zzz_arch_inner_cfma_f64
	.type	chord_zzz_arch_inner_cfma_f64, @function
chord_zzz_arch_inner_cfma_f64:
	vbroadcastsd %xmm0, %ymm2
	xorl	%ecx, %ecx
	movq	%rdx, %rax
	andq	$3, %rax
	shrq	$2, %rdx
	jmp	.L_loop_check
	.p2align 4,,10
	.p2align 4
	.L_loop:
	vmovapd	(%rsi, %rcx), %ymm0
	vmulpd	%ymm2, %ymm0, %ymm0
	vmovupd	(%rdi, %rcx), %ymm1
	vaddpd	%ymm1, %ymm0, %ymm0
	vmovupd	%ymm0, (%rdi, %rcx)
	leaq	32(%rcx), %rcx
	subq	$1, %rdx
	.L_loop_check:
	testq	%rdx, %rdx
	jne	.L_loop
	jmp	.L_loop_rem_check
	.p2align 4,,10
	.p2align 4
	.L_loop_rem:
	vmovsd	(%rsi, %rcx), %xmm0
	vmovsd	(%rdi, %rcx), %xmm1
	vmulsd	%xmm2, %xmm0, %xmm0
	vaddsd	%xmm1, %xmm0, %xmm0
	vmovsd	%xmm0, (%rdi, %rcx)
	leaq	8(%rcx), %rcx
	subq	$1, %rax
	.L_loop_rem_check:
	testq	%rax, %rax
	jne	.L_loop_rem
	vzeroupper
	retq
	.size	chord_zzz_arch_inner_cfma_f64, .-chord_zzz_arch_inner_cfma_f64
