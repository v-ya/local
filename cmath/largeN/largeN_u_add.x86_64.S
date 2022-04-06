.file	"largeN_u_add.x86_64.S"
.text

#include "largeN.64.h"

// uintptr_t largeN_u_add(largeN_s *restrict dst, uintptr_t u)
// rdi: dst
// rsi: u
	.p2align 4
	.globl	largeN_u_add
	.type	largeN_u_add, @function
largeN_u_add:
	// rdi: dst
	// rsi: u
	// rax: i
	// rdx: 0
	// rcx: n
	xorl	%eax, %eax
	movq	%rax, %rdx
	movq	largeN_offset_n(%rdi), %rcx
		.p2align 4,,10
		.p2align 4
		.L_loop:
			// dst->le[i] += u;
			addq	%rsi, largeN_offset_le(%rdi, %rax, 8)
			// u = last_add_is_carry;
			movq	%rdx, %rsi
			adcq	%rdx, %rsi
			// ++i
			leaq	1(%rax), %rax
			// if (i >= n) break;
			cmpq	%rcx, %rax
			jae	.L_check_u
			// if (u) continue;
			testq	%rsi, %rsi
			jne	.L_loop
	.L_loop_end:
		// if (dst->e < i) goto .L_fix_e;
		cmpq	%rax, largeN_offset_e(%rdi)
		jb	.L_fix_e
		// return u;
		movq	%rsi, %rax
		retq
	.p2align 4,,10
	.p2align 4
	.L_fix_e:
		// dst->e = i;
		movq	%rax, largeN_offset_e(%rdi)
		// return u;
		movq	%rsi, %rax
		retq
	.p2align 4,,10
	.p2align 4
	.L_check_u:
		// if (!u) goto .L_loop_end;
		testq	%rsi, %rsi
		je	.L_loop_end
		// largeN_measure_effective(dst);
		pushq	%rsi
		callq	largeN_measure_effective
		// return u;
		popq	%rax
		retq
	.size	largeN_u_add, .-largeN_u_add
