.file	"inner_vfma_f32s16.S"

.text

// void chord_zzz_arch_inner_vfma_f32s16(int16_t *restrict r, const float *restrict s, const float *restrict k, uintptr_t n, float kk)
// rdi: r
// rsi: s(32bytes align)
// rdx: k(32bytes align)
// rcx: n
// xmm0: kk
	.p2align 4
	.globl	chord_zzz_arch_inner_vfma_f32s16
	.hidden	chord_zzz_arch_inner_vfma_f32s16
	.type	chord_zzz_arch_inner_vfma_f32s16, @function
chord_zzz_arch_inner_vfma_f32s16:
	vbroadcastss %xmm0, %ymm2
	movq	%rcx, %r8
	andq	$7, %r8
	shrq	$3, %rcx
	xorl	%eax, %eax
	jmp	.L_loop_check
	.p2align 4,,10
	.p2align 4
	.L_loop:
	vmovaps	(%rsi, %rax, 2), %ymm0
	vmulps	%ymm2, %ymm0, %ymm0
	vmovaps	(%rdx, %rax, 2), %ymm1
	vmulps	%ymm1, %ymm0, %ymm0
	vpmovsxwd (%rdi, %rax), %ymm1
	vcvtps2dq %ymm0, %ymm0
	vpaddd	%ymm1, %ymm0, %ymm0
	vpshuflw $0xd8, %ymm0, %ymm0
	vpshufhw $0xd8, %ymm0, %ymm0
	vpshufd	$0xd8, %ymm0, %ymm0
	vmovq	%xmm0, (%rdi, %rax)
	vperm2f128 $0x01, %ymm0, %ymm0, %ymm0
	vmovq	%xmm0, 8(%rdi, %rax)
	leaq	16(%rax), %rax
	subq	$1, %rcx
	.L_loop_check:
	testq	%rcx, %rcx
	jne	.L_loop
	jmp	.L_loop_rem_check
	.p2align 4,,10
	.p2align 4
	.L_loop_rem:
	vmovss	(%rsi, %rax, 2), %xmm0
	vmulss	%xmm2, %xmm0, %xmm0
	vmovss	(%rdx, %rax, 2), %xmm1
	vmulss	%xmm1, %xmm0, %xmm0
	vcvtss2si %xmm0, %rcx
	addw	%cx, (%rdi, %rax)
	leaq	2(%rax), %rax
	subq	$1, %r8
	.L_loop_rem_check:
	testq	%r8, %r8
	jne	.L_loop_rem
	vzeroupper
	retq
	.size	chord_zzz_arch_inner_vfma_f32s16, .-chord_zzz_arch_inner_vfma_f32s16
