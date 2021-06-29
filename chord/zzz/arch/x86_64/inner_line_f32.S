.file	"inner_sin_4f32.S"

.text

// void chord_zzz_arch_inner_line_f32(float *restrict r, uintptr_t n, uintptr_t i, float k, float b)
// rdi: r(32bytes align)
// rsi: n
// rdx: i
// xmm0: k
// xmm1: b
	.p2align 4
	.globl	chord_zzz_arch_inner_line_f32
	.hidden	chord_zzz_arch_inner_line_f32
	.type	chord_zzz_arch_inner_line_f32, @function
chord_zzz_arch_inner_line_f32:
	addq	$7, %rsi
	shrq	$3, %rsi
	vbroadcastss %xmm0, %ymm3
	vbroadcastss %xmm1, %ymm4
	movq	$1, %rax
	vcvtsi2ss %rdx, %xmm0, %xmm0
	vmovss	%xmm0, (%rdi)
	addq	%rax, %rdx
	vcvtsi2ss %rdx, %xmm0, %xmm0
	vmovss	%xmm0, 4(%rdi)
	addq	%rax, %rdx
	vcvtsi2ss %rdx, %xmm0, %xmm0
	vmovss	%xmm0, 8(%rdi)
	addq	%rax, %rdx
	vcvtsi2ss %rdx, %xmm0, %xmm0
	vmovss	%xmm0, 12(%rdi)
	addq	%rax, %rdx
	vcvtsi2ss %rdx, %xmm0, %xmm0
	vmovss	%xmm0, 16(%rdi)
	addq	%rax, %rdx
	vcvtsi2ss %rdx, %xmm0, %xmm0
	vmovss	%xmm0, 20(%rdi)
	addq	%rax, %rdx
	vcvtsi2ss %rdx, %xmm0, %xmm0
	vmovss	%xmm0, 24(%rdi)
	addq	%rax, %rdx
	vcvtsi2ss %rdx, %xmm0, %xmm0
	vmovss	%xmm0, 28(%rdi)
	movq	$8, %rax
	vcvtsi2ss %rax, %xmm0, %xmm0
	vmovaps	(%rdi), %ymm1
	vbroadcastss %xmm0, %ymm2
	.p2align 4,,10
	.p2align 4
	.L_loop:
	vmulps	%ymm3, %ymm1, %ymm0
	vaddps	%ymm4, %ymm0, %ymm0
	vmovaps	%ymm0, (%rdi)
	vaddps	%ymm2, %ymm1, %ymm1
	addq	$32, %rdi
	subq	$1, %rsi
	testq	%rsi, %rsi
	jne	.L_loop
	vzeroupper
	retq
	.size	chord_zzz_arch_inner_line_f32, .-chord_zzz_arch_inner_line_f32
