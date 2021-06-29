.file	"inner_vfma_f64s32.S"

.text

// void chord_zzz_arch_inner_vfma_f64s32(int32_t *restrict r, const double *restrict s, const double *restrict k, uintptr_t n, double kk)
// rdi: r
// rsi: s(32bytes align)
// rdx: k(32bytes align)
// rcx: n
// xmm0: kk
	.p2align 4
	.globl	chord_zzz_arch_inner_vfma_f64s32
	.hidden	chord_zzz_arch_inner_vfma_f64s32
	.type	chord_zzz_arch_inner_vfma_f64s32, @function
chord_zzz_arch_inner_vfma_f64s32:
	vbroadcastsd %xmm0, %ymm2
	movq	%rcx, %r8
	andq	$3, %r8
	shrq	$2, %rcx
	xorl	%eax, %eax
	jmp	.L_loop_check
	.p2align 4,,10
	.p2align 4
	.L_loop:
	vmovapd	(%rsi, %rax, 2), %ymm0
	vmulpd	%ymm2, %ymm0, %ymm0
	vmovapd	(%rdx, %rax, 2), %ymm1
	vmulpd	%ymm1, %ymm0, %ymm0
	vmovdqu	(%rdi, %rax), %xmm1
	vcvtpd2dq %ymm0, %xmm0
	vpaddd	%xmm1, %xmm0, %xmm0
	vmovdqu	%xmm0, (%rdi, %rax)
	leaq	16(%rax), %rax
	subq	$1, %rcx
	.L_loop_check:
	testq	%rcx, %rcx
	jne	.L_loop
	jmp	.L_loop_rem_check
	.p2align 4,,10
	.p2align 4
	.L_loop_rem:
	vmovsd	(%rsi, %rax, 2), %xmm0
	vmulsd	%xmm2, %xmm0, %xmm0
	vmovsd	(%rdx, %rax, 2), %xmm1
	vmulsd	%xmm1, %xmm0, %xmm0
	vcvtsd2si %xmm0, %rcx
	addl	%ecx, (%rdi, %rax)
	leaq	4(%rax), %rax
	subq	$1, %r8
	.L_loop_rem_check:
	testq	%r8, %r8
	jne	.L_loop_rem
	vzeroupper
	retq
	.size	chord_zzz_arch_inner_vfma_f64s32, .-chord_zzz_arch_inner_vfma_f64s32
