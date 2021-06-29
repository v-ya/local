.file	"add_sin_nf32.S"

.section	.rodata.cst4, "aM", @progbits, 4
	.align 4
.L_2_pi:	.float	6.36619772367581343076e-01
.L_pi_2:	.float	1.57079632679489661923e-00

.section	.rodata.cst8, "aM", @progbits, 8
	.align 8
.L_mask:	.quad	0x0303030303030303

.text

// void chord_zzz_arch_inner_sin_nf32(float *restrict r, const float *restrict v, uintptr_t block, uintptr_t n)
// rdi: r     (32bytes align)
// rsi: v     (32bytes align)
// rdx: block (must block && block % 8 == 0)
// rcx: n     (must n <= block)
	.p2align 4
	.globl	chord_zzz_arch_inner_sin_nf32
	.hidden	chord_zzz_arch_inner_sin_nf32
	.type	chord_zzz_arch_inner_sin_nf32, @function
chord_zzz_arch_inner_sin_nf32:
	pushq	%rbp
	pushq	%rbx
	pushq	%r12
	pushq	%r13
	movq	%rsp, %rbp
	shrq	$5, %rsp
	shlq	$5, %rsp
	shlq	$5, %rdx
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
	movl	$0x80000000, %r12d
	movq	%rdi, %r13
	xorl	%edi, %edi
	// load ymm1, ymm2, ymm3
	vbroadcastss	.L_2_pi(%rip), %ymm1
	vbroadcastss	.L_pi_2(%rip), %ymm2
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
		vmovaps	(%rsi), %ymm0
		addq	$32, %rsi
		vmulps	%ymm1, %ymm0, %ymm4
		vcvtps2dq %ymm4, %ymm4
		vcvtdq2ps %ymm4, %ymm5
		vmulps	%ymm2, %ymm5, %ymm5
		vsubps	%ymm5, %ymm0, %ymm0
		vpand	%ymm3, %ymm4, %ymm4

		vmovd	%xmm4, %eax
		leaq	.L_loop_check_1(%rip), %rbx
		jmpq	*-32(%rsp, %rax, 8)
		.L_loop_check_1:
		vpermilps $0x39, %ymm0, %ymm0
		vpshufd	$0x39, %ymm4, %ymm4

		vmovd	%xmm4, %eax
		leaq	.L_loop_check_2(%rip), %rbx
		jmpq	*-32(%rsp, %rax, 8)
		.L_loop_check_2:
		vpermilps $0x39, %ymm0, %ymm0
		vpshufd	$0x39, %ymm4, %ymm4

		vmovd	%xmm4, %eax
		leaq	.L_loop_check_3(%rip), %rbx
		jmpq	*-32(%rsp, %rax, 8)
		.L_loop_check_3:
		vpermilps $0x39, %ymm0, %ymm0
		vpshufd	$0x39, %ymm4, %ymm4

		vmovd	%xmm4, %eax
		leaq	.L_loop_check_4(%rip), %rbx
		jmpq	*-32(%rsp, %rax, 8)
		.L_loop_check_4:
		vpermilps $0x39, %ymm0, %ymm0
		vperm2f128 $0x01, %ymm0, %ymm0, %ymm0
		vpshufd	$0x39, %ymm4, %ymm4
		vperm2f128 $0x01, %ymm4, %ymm4, %ymm4

		vmovd	%xmm4, %eax
		leaq	.L_loop_check_5(%rip), %rbx
		jmpq	*-32(%rsp, %rax, 8)
		.L_loop_check_5:
		vpermilps $0x39, %ymm0, %ymm0
		vpshufd	$0x39, %ymm4, %ymm4

		vmovd	%xmm4, %eax
		leaq	.L_loop_check_6(%rip), %rbx
		jmpq	*-32(%rsp, %rax, 8)
		.L_loop_check_6:
		vpermilps $0x39, %ymm0, %ymm0
		vpshufd	$0x39, %ymm4, %ymm4

		vmovd	%xmm4, %eax
		leaq	.L_loop_check_7(%rip), %rbx
		jmpq	*-32(%rsp, %rax, 8)
		.L_loop_check_7:
		vpermilps $0x39, %ymm0, %ymm0
		vpshufd	$0x39, %ymm4, %ymm4

		vmovd	%xmm4, %eax
		leaq	.L_loop_check(%rip), %rbx
		jmpq	*-32(%rsp, %rax, 8)
	.p2align 4,,10
	.p2align 4
	.L_calc_start:
	leaq	-32(%rsp), %rdi
	movq	%rsp, %rbx
	movq	%rsp, %rsi
	movq	%rdi, %rsp
	callq	chord_zzz_arch_inner_sin_8f32_load_coff
	jmp	.L_calc_n1_check

	.p2align 4,,10
	.p2align 4
	.L_calc_n1:
		callq	chord_zzz_arch_inner_sin_8f32
		movl	(%rsi, %rdx, 4), %ecx
		movl	(%rdi), %eax
		movl	%eax, (%r13, %rcx, 4)
		leaq	4(%rsi), %rsi
		cmpq	%rsi, %r8
		jbe	.L_calc_n1_end
		movl	(%rsi, %rdx, 4), %ecx
		movl	4(%rdi), %eax
		movl	%eax, (%r13, %rcx, 4)
		leaq	4(%rsi), %rsi
		cmpq	%rsi, %r8
		jbe	.L_calc_n1_end
		movl	(%rsi, %rdx, 4), %ecx
		movl	8(%rdi), %eax
		movl	%eax, (%r13, %rcx, 4)
		leaq	4(%rsi), %rsi
		cmpq	%rsi, %r8
		jbe	.L_calc_n1_end
		movl	(%rsi, %rdx, 4), %ecx
		movl	12(%rdi), %eax
		movl	%eax, (%r13, %rcx, 4)
		leaq	4(%rsi), %rsi
		cmpq	%rsi, %r8
		jbe	.L_calc_n1_end
		movl	(%rsi, %rdx, 4), %ecx
		movl	16(%rdi), %eax
		movl	%eax, (%r13, %rcx, 4)
		leaq	4(%rsi), %rsi
		cmpq	%rsi, %r8
		jbe	.L_calc_n1_end
		movl	(%rsi, %rdx, 4), %ecx
		movl	20(%rdi), %eax
		movl	%eax, (%r13, %rcx, 4)
		leaq	4(%rsi), %rsi
		cmpq	%rsi, %r8
		jbe	.L_calc_n1_end
		movl	(%rsi, %rdx, 4), %ecx
		movl	24(%rdi), %eax
		movl	%eax, (%r13, %rcx, 4)
		leaq	4(%rsi), %rsi
		cmpq	%rsi, %r8
		jbe	.L_calc_n1_end
		movl	(%rsi, %rdx, 4), %ecx
		movl	28(%rdi), %eax
		movl	%eax, (%r13, %rcx, 4)
		leaq	4(%rsi), %rsi
		.L_calc_n1_check:
		cmpq	%rsi, %r8
		ja	.L_calc_n1
		.L_calc_n1_end:

	leaq	(%rbx, %rdx, 2), %rsi
	jmp	.L_calc_n3_check
	.p2align 4,,10
	.p2align 4
	.L_calc_n3:
		callq	chord_zzz_arch_inner_sin_8f32
		movl	(%rsi, %rdx, 4), %ecx
		movl	(%rdi), %eax
		xorl	%r12d, %eax
		movl	%eax, (%r13, %rcx, 4)
		leaq	4(%rsi), %rsi
		cmpq	%rsi, %r10
		jbe	.L_calc_n3_end
		movl	(%rsi, %rdx, 4), %ecx
		movl	4(%rdi), %eax
		xorl	%r12d, %eax
		movl	%eax, (%r13, %rcx, 4)
		leaq	4(%rsi), %rsi
		cmpq	%rsi, %r10
		jbe	.L_calc_n3_end
		movl	(%rsi, %rdx, 4), %ecx
		movl	8(%rdi), %eax
		xorl	%r12d, %eax
		movl	%eax, (%r13, %rcx, 4)
		leaq	4(%rsi), %rsi
		cmpq	%rsi, %r10
		jbe	.L_calc_n3_end
		movl	(%rsi, %rdx, 4), %ecx
		movl	12(%rdi), %eax
		xorl	%r12d, %eax
		movl	%eax, (%r13, %rcx, 4)
		leaq	4(%rsi), %rsi
		cmpq	%rsi, %r10
		jbe	.L_calc_n3_end
		movl	(%rsi, %rdx, 4), %ecx
		movl	16(%rdi), %eax
		xorl	%r12d, %eax
		movl	%eax, (%r13, %rcx, 4)
		leaq	4(%rsi), %rsi
		cmpq	%rsi, %r10
		jbe	.L_calc_n3_end
		movl	(%rsi, %rdx, 4), %ecx
		movl	20(%rdi), %eax
		xorl	%r12d, %eax
		movl	%eax, (%r13, %rcx, 4)
		leaq	4(%rsi), %rsi
		cmpq	%rsi, %r10
		jbe	.L_calc_n3_end
		movl	(%rsi, %rdx, 4), %ecx
		movl	24(%rdi), %eax
		xorl	%r12d, %eax
		movl	%eax, (%r13, %rcx, 4)
		leaq	4(%rsi), %rsi
		cmpq	%rsi, %r10
		jbe	.L_calc_n3_end
		movl	(%rsi, %rdx, 4), %ecx
		movl	28(%rdi), %eax
		xorl	%r12d, %eax
		movl	%eax, (%r13, %rcx, 4)
		leaq	4(%rsi), %rsi
		.L_calc_n3_check:
		cmpq	%rsi, %r10
		ja	.L_calc_n3
		.L_calc_n3_end:

	addq	%rdx, %rbx
	movq	%rbx, %rsi
	callq	chord_zzz_arch_inner_cos_8f32_load_coff
	jmp	.L_calc_n2_check
	.p2align 4,,10
	.p2align 4
	.L_calc_n2:
		callq	chord_zzz_arch_inner_cos_8f32
		movl	(%rsi, %rdx, 4), %ecx
		movl	(%rdi), %eax
		movl	%eax, (%r13, %rcx, 4)
		leaq	4(%rsi), %rsi
		cmpq	%rsi, %r9
		jbe	.L_calc_n2_end
		movl	(%rsi, %rdx, 4), %ecx
		movl	4(%rdi), %eax
		movl	%eax, (%r13, %rcx, 4)
		leaq	4(%rsi), %rsi
		cmpq	%rsi, %r9
		jbe	.L_calc_n2_end
		movl	(%rsi, %rdx, 4), %ecx
		movl	8(%rdi), %eax
		movl	%eax, (%r13, %rcx, 4)
		leaq	4(%rsi), %rsi
		cmpq	%rsi, %r9
		jbe	.L_calc_n2_end
		movl	(%rsi, %rdx, 4), %ecx
		movl	12(%rdi), %eax
		movl	%eax, (%r13, %rcx, 4)
		leaq	4(%rsi), %rsi
		cmpq	%rsi, %r9
		jbe	.L_calc_n2_end
		movl	(%rsi, %rdx, 4), %ecx
		movl	16(%rdi), %eax
		movl	%eax, (%r13, %rcx, 4)
		leaq	4(%rsi), %rsi
		cmpq	%rsi, %r9
		jbe	.L_calc_n2_end
		movl	(%rsi, %rdx, 4), %ecx
		movl	20(%rdi), %eax
		movl	%eax, (%r13, %rcx, 4)
		leaq	4(%rsi), %rsi
		cmpq	%rsi, %r9
		jbe	.L_calc_n2_end
		movl	(%rsi, %rdx, 4), %ecx
		movl	24(%rdi), %eax
		movl	%eax, (%r13, %rcx, 4)
		leaq	4(%rsi), %rsi
		cmpq	%rsi, %r9
		jbe	.L_calc_n2_end
		movl	(%rsi, %rdx, 4), %ecx
		movl	28(%rdi), %eax
		movl	%eax, (%r13, %rcx, 4)
		leaq	4(%rsi), %rsi
		.L_calc_n2_check:
		cmpq	%rsi, %r9
		ja	.L_calc_n2
		.L_calc_n2_end:

	leaq	(%rbx, %rdx, 2), %rsi
	jmp	.L_calc_n4_check
	.p2align 4,,10
	.p2align 4
	.L_calc_n4:
		callq	chord_zzz_arch_inner_cos_8f32
		movl	(%rsi, %rdx, 4), %ecx
		movl	(%rdi), %eax
		xorl	%r12d, %eax
		movl	%eax, (%r13, %rcx, 4)
		leaq	4(%rsi), %rsi
		cmpq	%rsi, %r11
		jbe	.L_calc_n4_end
		movl	(%rsi, %rdx, 4), %ecx
		movl	4(%rdi), %eax
		xorl	%r12d, %eax
		movl	%eax, (%r13, %rcx, 4)
		leaq	4(%rsi), %rsi
		cmpq	%rsi, %r11
		jbe	.L_calc_n4_end
		movl	(%rsi, %rdx, 4), %ecx
		movl	8(%rdi), %eax
		xorl	%r12d, %eax
		movl	%eax, (%r13, %rcx, 4)
		leaq	4(%rsi), %rsi
		cmpq	%rsi, %r11
		jbe	.L_calc_n4_end
		movl	(%rsi, %rdx, 4), %ecx
		movl	12(%rdi), %eax
		xorl	%r12d, %eax
		movl	%eax, (%r13, %rcx, 4)
		leaq	4(%rsi), %rsi
		cmpq	%rsi, %r11
		jbe	.L_calc_n4_end
		movl	(%rsi, %rdx, 4), %ecx
		movl	16(%rdi), %eax
		xorl	%r12d, %eax
		movl	%eax, (%r13, %rcx, 4)
		leaq	4(%rsi), %rsi
		cmpq	%rsi, %r11
		jbe	.L_calc_n4_end
		movl	(%rsi, %rdx, 4), %ecx
		movl	20(%rdi), %eax
		xorl	%r12d, %eax
		movl	%eax, (%r13, %rcx, 4)
		leaq	4(%rsi), %rsi
		cmpq	%rsi, %r11
		jbe	.L_calc_n4_end
		movl	(%rsi, %rdx, 4), %ecx
		movl	24(%rdi), %eax
		xorl	%r12d, %eax
		movl	%eax, (%r13, %rcx, 4)
		leaq	4(%rsi), %rsi
		cmpq	%rsi, %r11
		jbe	.L_calc_n4_end
		movl	(%rsi, %rdx, 4), %ecx
		movl	28(%rdi), %eax
		xorl	%r12d, %eax
		movl	%eax, (%r13, %rcx, 4)
		leaq	4(%rsi), %rsi
		.L_calc_n4_check:
		cmpq	%rsi, %r11
		ja	.L_calc_n4
		.L_calc_n4_end:

	movq	%rbp, %rsp
	popq	%r13
	popq	%r12
	popq	%rbx
	popq	%rbp
	vzeroupper
	retq
	.p2align 4,,10
	.p2align 4
	.L_case_n1:
		vmovss	%xmm0, (%r8)
		movl	%edi, (%r8, %rdx, 4)
		leaq	1(%rdi), %rdi
		leaq	4(%r8), %r8
		cmpq	%rdi, %rcx
		jbe	.L_calc_start
		jmpq	*%rbx
	.p2align 4,,10
	.p2align 4
	.L_case_n2:
		vmovss	%xmm0, (%r9)
		movl	%edi, (%r9, %rdx, 4)
		leaq	1(%rdi), %rdi
		leaq	4(%r9), %r9
		cmpq	%rdi, %rcx
		jbe	.L_calc_start
		jmpq	*%rbx
	.p2align 4,,10
	.p2align 4
	.L_case_n3:
		vmovss	%xmm0, (%r10)
		movl	%edi, (%r10, %rdx, 4)
		leaq	1(%rdi), %rdi
		leaq	4(%r10), %r10
		cmpq	%rdi, %rcx
		jbe	.L_calc_start
		jmpq	*%rbx
	.p2align 4,,10
	.p2align 4
	.L_case_n4:
		vmovss	%xmm0, (%r11)
		movl	%edi, (%r11, %rdx, 4)
		leaq	1(%rdi), %rdi
		leaq	4(%r11), %r11
		cmpq	%rdi, %rcx
		jbe	.L_calc_start
		jmpq	*%rbx
	.p2align 4,,10
	.p2align 4

	.size	chord_zzz_arch_inner_sin_nf32, .-chord_zzz_arch_inner_sin_nf32
