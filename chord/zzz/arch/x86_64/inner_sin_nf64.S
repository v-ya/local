.file	"add_sin_nf64.S"

.section	.rodata.cst8, "aM", @progbits, 8
	.align 8
.L_2_pi:	.double	6.36619772367581343076e-01
.L_pi_2:	.double	1.57079632679489661923e-00
.L_mask:	.long	0x0000000003030303

.text

// void chord_zzz_arch_inner_sin_nf64(double *restrict r, const double *restrict v, uintptr_t block, uintptr_t n)
// rdi: r     (32bytes align)
// rsi: v     (32bytes align)
// rdx: block (must block && block % 4 == 0)
// rcx: n     (must n <= block)
	.p2align 4
	.globl	chord_zzz_arch_inner_sin_nf64
	.hidden	chord_zzz_arch_inner_sin_nf64
	.type	chord_zzz_arch_inner_sin_nf64, @function
chord_zzz_arch_inner_sin_nf64:
	pushq	%rbp
	pushq	%rbx
	pushq	%r12
	movq	%rsp, %rbp
	shrq	$5, %rsp
	shlq	$5, %rsp
	shlq	$6, %rdx
	subq	%rdx, %rsp
	// set r8-r11
	shrq	$3, %rdx
	movq	%rsp, %rax
	movq	%rax, %r8
	addq	%rdx, %rax
	movq	%rax, %r9
	addq	%rdx, %rax
	movq	%rax, %r10
	addq	%rdx, %rax
	movq	%rax, %r11
	movq	$0x8000000000000000, %r12
	// load xmm1, xmm2
	vbroadcastsd	.L_2_pi(%rip), %ymm1
	vbroadcastsd	.L_pi_2(%rip), %ymm2
	vpmovzxbd	.L_mask(%rip), %ymm3
	// set jmp table
	leaq	.L_case_n1(%rip), %rax
	movq	%rax, -32(%rsp)
	leaq	.L_case_n2(%rip), %rax
	movq	%rax, -24(%rsp)
	leaq	.L_case_n3(%rip), %rax
	movq	%rax, -16(%rsp)
	leaq	.L_case_n4(%rip), %rax
	movq	%rax, -8(%rsp)
	// loop
	.p2align 4,,10
	.p2align 4
	.L_loop_check:
		vmovapd	(%rsi), %ymm0
		addq	$32, %rsi
		vmulpd	%ymm1, %ymm0, %ymm4
		vcvtpd2dq %ymm4, %xmm4
		vcvtdq2pd %xmm4, %ymm5
		vmulpd	%ymm2, %ymm5, %ymm5
		vsubpd	%ymm5, %ymm0, %ymm0
		vpand	%ymm3, %ymm4, %ymm4

		movd	%xmm4, %eax
		leaq	.L_loop_check_1(%rip), %rbx
		jmpq	*-32(%rsp, %rax, 8)
		.L_loop_check_1:
		vpermilpd $0x09, %ymm0, %ymm0
		vpshufd	$0x39, %ymm4, %ymm4

		movd	%xmm4, %eax
		leaq	.L_loop_check_2(%rip), %rbx
		jmpq	*-32(%rsp, %rax, 8)
		.L_loop_check_2:
		vperm2f128 $0x01, %ymm0, %ymm0, %ymm0
		vpshufd	$0x39, %ymm4, %ymm4

		movd	%xmm4, %eax
		leaq	.L_loop_check_3(%rip), %rbx
		jmpq	*-32(%rsp, %rax, 8)
		.L_loop_check_3:
		vpermilpd $0x09, %ymm0, %ymm0
		vpshufd	$0x39, %ymm4, %ymm4

		movd	%xmm4, %eax
		leaq	.L_loop_check(%rip), %rbx
		jmpq	*-32(%rsp, %rax, 8)
	.p2align 4,,10
	.p2align 4
	.L_calc_start:
	leaq	-32(%rsp), %rdi
	movq	%rsp, %rbx
	movq	%rsp, %rsi
	movq	%rdi, %rsp
	callq	chord_zzz_arch_inner_sin_4f64_load_coff
	jmp	.L_calc_n1_check

	.p2align 4,,10
	.p2align 4
	.L_calc_n1:
		callq	chord_zzz_arch_inner_sin_4f64
		movq	(%rsi, %rdx, 4), %rcx
		movq	(%rdi), %rax
		movq	%rax, (%rcx)
		leaq	8(%rsi), %rsi
		cmpq	%rsi, %r8
		jbe	.L_calc_n1_end
		movq	(%rsi, %rdx, 4), %rcx
		movq	8(%rdi), %rax
		movq	%rax, (%rcx)
		leaq	8(%rsi), %rsi
		cmpq	%rsi, %r8
		jbe	.L_calc_n1_end
		movq	(%rsi, %rdx, 4), %rcx
		movq	16(%rdi), %rax
		movq	%rax, (%rcx)
		leaq	8(%rsi), %rsi
		cmpq	%rsi, %r8
		jbe	.L_calc_n1_end
		movq	(%rsi, %rdx, 4), %rcx
		movq	24(%rdi), %rax
		movq	%rax, (%rcx)
		leaq	8(%rsi), %rsi
		.L_calc_n1_check:
		cmpq	%rsi, %r8
		ja	.L_calc_n1
		.L_calc_n1_end:

	leaq	(%rbx, %rdx, 2), %rsi
	jmp	.L_calc_n3_check
	.p2align 4,,10
	.p2align 4
	.L_calc_n3:
		callq	chord_zzz_arch_inner_sin_4f64
		movq	(%rsi, %rdx, 4), %rcx
		movq	(%rdi), %rax
		xorq	%r12, %rax
		movq	%rax, (%rcx)
		leaq	8(%rsi), %rsi
		cmpq	%rsi, %r10
		jbe	.L_calc_n3_end
		movq	(%rsi, %rdx, 4), %rcx
		movq	8(%rdi), %rax
		xorq	%r12, %rax
		movq	%rax, (%rcx)
		leaq	8(%rsi), %rsi
		cmpq	%rsi, %r10
		jbe	.L_calc_n3_end
		movq	(%rsi, %rdx, 4), %rcx
		movq	16(%rdi), %rax
		xorq	%r12, %rax
		movq	%rax, (%rcx)
		leaq	8(%rsi), %rsi
		cmpq	%rsi, %r10
		jbe	.L_calc_n3_end
		movq	(%rsi, %rdx, 4), %rcx
		movq	24(%rdi), %rax
		xorq	%r12, %rax
		movq	%rax, (%rcx)
		leaq	8(%rsi), %rsi
		.L_calc_n3_check:
		cmpq	%rsi, %r10
		ja	.L_calc_n3
		.L_calc_n3_end:

	addq	%rdx, %rbx
	movq	%rbx, %rsi
	callq	chord_zzz_arch_inner_cos_4f64_load_coff
	jmp	.L_calc_n2_check
	.p2align 4,,10
	.p2align 4
	.L_calc_n2:
		callq	chord_zzz_arch_inner_cos_4f64
		movq	(%rsi, %rdx, 4), %rcx
		movq	(%rdi), %rax
		movq	%rax, (%rcx)
		leaq	8(%rsi), %rsi
		cmpq	%rsi, %r9
		jbe	.L_calc_n2_end
		movq	(%rsi, %rdx, 4), %rcx
		movq	8(%rdi), %rax
		movq	%rax, (%rcx)
		leaq	8(%rsi), %rsi
		cmpq	%rsi, %r9
		jbe	.L_calc_n2_end
		movq	(%rsi, %rdx, 4), %rcx
		movq	16(%rdi), %rax
		movq	%rax, (%rcx)
		leaq	8(%rsi), %rsi
		cmpq	%rsi, %r9
		jbe	.L_calc_n2_end
		movq	(%rsi, %rdx, 4), %rcx
		movq	24(%rdi), %rax
		movq	%rax, (%rcx)
		leaq	8(%rsi), %rsi
		.L_calc_n2_check:
		cmpq	%rsi, %r9
		ja	.L_calc_n2
		.L_calc_n2_end:

	leaq	(%rbx, %rdx, 2), %rsi
	jmp	.L_calc_n4_check
	.p2align 4,,10
	.p2align 4
	.L_calc_n4:
		callq	chord_zzz_arch_inner_cos_4f64
		movq	(%rsi, %rdx, 4), %rcx
		movq	(%rdi), %rax
		xorq	%r12, %rax
		movq	%rax, (%rcx)
		leaq	8(%rsi), %rsi
		cmpq	%rsi, %r11
		jbe	.L_calc_n4_end
		movq	(%rsi, %rdx, 4), %rcx
		movq	8(%rdi), %rax
		xorq	%r12, %rax
		movq	%rax, (%rcx)
		leaq	8(%rsi), %rsi
		cmpq	%rsi, %r11
		jbe	.L_calc_n4_end
		movq	(%rsi, %rdx, 4), %rcx
		movq	16(%rdi), %rax
		xorq	%r12, %rax
		movq	%rax, (%rcx)
		leaq	8(%rsi), %rsi
		cmpq	%rsi, %r11
		jbe	.L_calc_n4_end
		movq	(%rsi, %rdx, 4), %rcx
		movq	24(%rdi), %rax
		xorq	%r12, %rax
		movq	%rax, (%rcx)
		leaq	8(%rsi), %rsi
		.L_calc_n4_check:
		cmpq	%rsi, %r11
		ja	.L_calc_n4
		.L_calc_n4_end:

	movq	%rbp, %rsp
	popq	%r12
	popq	%rbx
	popq	%rbp
	retq
	.p2align 4,,10
	.p2align 4
	.L_case_n1:
		movsd	%xmm0, (%r8)
		movq	%rdi, (%r8, %rdx, 4)
		leaq	8(%rdi), %rdi
		leaq	8(%r8), %r8
		subq	$1, %rcx
		testq	%rcx, %rcx
		je	.L_calc_start
		jmpq	*%rbx
	.p2align 4,,10
	.p2align 4
	.L_case_n2:
		movsd	%xmm0, (%r9)
		movq	%rdi, (%r9, %rdx, 4)
		leaq	8(%rdi), %rdi
		leaq	8(%r9), %r9
		subq	$1, %rcx
		testq	%rcx, %rcx
		je	.L_calc_start
		jmpq	*%rbx
	.p2align 4,,10
	.p2align 4
	.L_case_n3:
		movsd	%xmm0, (%r10)
		movq	%rdi, (%r10, %rdx, 4)
		leaq	8(%rdi), %rdi
		leaq	8(%r10), %r10
		subq	$1, %rcx
		testq	%rcx, %rcx
		je	.L_calc_start
		jmpq	*%rbx
	.p2align 4,,10
	.p2align 4
	.L_case_n4:
		movsd	%xmm0, (%r11)
		movq	%rdi, (%r11, %rdx, 4)
		leaq	8(%rdi), %rdi
		leaq	8(%r11), %r11
		subq	$1, %rcx
		testq	%rcx, %rcx
		je	.L_calc_start
		jmpq	*%rbx
	.p2align 4,,10
	.p2align 4

	.size	chord_zzz_arch_inner_sin_nf64, .-chord_zzz_arch_inner_sin_nf64
