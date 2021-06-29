.file	"inner_cfma_f32s16.S"

.text

// void chord_zzz_arch_inner_cfma_f32s16(int16_t *restrict r, const float *restrict s, uintptr_t n, float k)
// rdi: r
// rsi: s(32bytes align)
// rdx: n
// xmm0: k
	.p2align 4
	.globl	chord_zzz_arch_inner_cfma_f32s16
	.hidden	chord_zzz_arch_inner_cfma_f32s16
	.type	chord_zzz_arch_inner_cfma_f32s16, @function
chord_zzz_arch_inner_cfma_f32s16:
	vbroadcastss %xmm0, %ymm2
	movq	%rdx, %rax
	andq	$7, %rax
	shrq	$3, %rdx
	jmp	.L_loop_check
	.p2align 4,,10
	.p2align 4
	.L_loop:
	vmovaps	(%rsi), %ymm0
	vmulps	%ymm2, %ymm0, %ymm0
	vpmovsxwd (%rdi), %ymm1
	vcvtps2dq %ymm0, %ymm0
	vpaddd	%ymm1, %ymm0, %ymm0
	vpshuflw $0xd8, %ymm0, %ymm0
	vpshufhw $0xd8, %ymm0, %ymm0
	vpshufd	$0xd8, %ymm0, %ymm0
	vmovq	%xmm0, (%rdi)
	vperm2f128 $0x01, %ymm0, %ymm0, %ymm0
	vmovq	%xmm0, 8(%rdi)
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
	vmovss	(%rsi), %xmm0
	vmulss	%xmm2, %xmm0, %xmm0
	vcvtss2si %xmm0, %rdx
	addw	%dx, (%rdi)
	leaq	4(%rsi), %rsi
	leaq	2(%rdi), %rdi
	subq	$1, %rax
	.L_loop_rem_check:
	testq	%rax, %rax
	jne	.L_loop_rem
	vzeroupper
	retq
	.size	chord_zzz_arch_inner_cfma_f32s16, .-chord_zzz_arch_inner_cfma_f32s16
