.file	"inner_cfma_f64s32.S"

.text

// void chord_zzz_arch_inner_cfma_f64s32(int32_t *restrict r, const double *restrict s, uintptr_t n, double k)
// rdi: r
// rsi: s(32bytes align)
// rdx: n
// xmm0: k
	.p2align 4
	.globl	chord_zzz_arch_inner_cfma_f64s32
	.hidden	chord_zzz_arch_inner_cfma_f64s32
	.type	chord_zzz_arch_inner_cfma_f64s32, @function
chord_zzz_arch_inner_cfma_f64s32:
	vbroadcastsd %xmm0, %ymm2
	movq	%rdx, %rax
	andq	$3, %rax
	shrq	$2, %rdx
	jmp	.L_loop_check
	.p2align 4,,10
	.p2align 4
	.L_loop:
	vmovapd	(%rsi), %ymm0
	vmovdqu	(%rdi), %xmm1
	vmulpd	%ymm2, %ymm0, %ymm0
	vcvtpd2dq %ymm0, %xmm0
	vpaddd	%xmm1, %xmm0, %xmm0
	vmovdqu	%xmm0, (%rdi)
	leaq	32(%rsi), %rsi
	leaq	16(%rdi), %rdi
	subq	$1, %rdx
	.L_loop_check:
	testq	%rdx, %rdx
	jne	.L_loop
	jmp	.L_loop_rem_check
	.p2align 4,,10
	.p2align 4
	.L_loop_rem:
	movsd	(%rsi), %xmm0
	mulsd	%xmm2, %xmm0
	cvtsd2si %xmm0, %rdx
	addl	%edx, (%rdi)
	leaq	8(%rsi), %rsi
	leaq	4(%rdi), %rdi
	subq	$1, %rax
	.L_loop_rem_check:
	testq	%rax, %rax
	jne	.L_loop_rem
	retq
	.size	chord_zzz_arch_inner_cfma_f64s32, .-chord_zzz_arch_inner_cfma_f64s32
