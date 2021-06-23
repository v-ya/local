.file	"inner_sin_4f64.S"

.text

// void chord_zzz_arch_inner_line_f64(double *restrict r, uintptr_t n, uintptr_t i, double k, double b)
// rdi: r(32bytes align)
// rsi: n
// rdx: i
// xmm0: k
// xmm1: b
	.p2align 4
	.globl	chord_zzz_arch_inner_line_f64
	.hidden	chord_zzz_arch_inner_line_f64
	.type	chord_zzz_arch_inner_line_f64, @function
chord_zzz_arch_inner_line_f64:
	addq	$3, %rsi
	shrq	$2, %rsi
	vbroadcastsd %xmm0, %ymm3
	vbroadcastsd %xmm1, %ymm4
	movq	$1, %rax
	cvtsi2sd %rdx, %xmm0
	movsd	%xmm0, (%rdi)
	addq	%rax, %rdx
	cvtsi2sd %rdx, %xmm0
	movsd	%xmm0, 8(%rdi)
	addq	%rax, %rdx
	cvtsi2sd %rdx, %xmm0
	movsd	%xmm0, 16(%rdi)
	addq	%rax, %rdx
	cvtsi2sd %rdx, %xmm0
	movsd	%xmm0, 24(%rdi)
	movq	$4, %rax
	cvtsi2sd %rax, %xmm0
	vmovapd	(%rdi), %ymm1
	vbroadcastsd %xmm0, %ymm2
	.p2align 4,,10
	.p2align 4
	.L_loop:
	vmulpd	%ymm3, %ymm1, %ymm0
	vaddpd	%ymm4, %ymm0, %ymm0
	vmovapd	%ymm0, (%rdi)
	vaddpd	%ymm2, %ymm1, %ymm1
	addq	$32, %rdi
	subq	$1, %rsi
	testq	%rsi, %rsi
	jne	.L_loop
	retq
	.size	chord_zzz_arch_inner_line_f64, .-chord_zzz_arch_inner_line_f64
