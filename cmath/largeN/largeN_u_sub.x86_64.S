.file	"largeN_u_sub.x86_64.S"
.text

#include "largeN.64.h"

// uintptr_t largeN_u_sub(largeN_s *restrict dst, uintptr_t u)
// rdi: dst
// rsi: u
	.p2align 4
	.globl	largeN_u_sub
	.type	largeN_u_sub, @function
largeN_u_sub:
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
			subq	%rsi, largeN_offset_le(%rdi, %rax, 8)
			// u = last_add_is_carry;
			movq	%rdx, %rsi
			adcq	%rdx, %rsi
			// ++i
			leaq	1(%rax), %rax
			// if (i >= n) break;
			cmpq	%rcx, %rax
			jae	.L_loop_end
			// if (u) continue;
			testq	%rsi, %rsi
			jne	.L_loop
	.L_loop_end:
	// if (u || (i && i == dst->e && !dst->le[i - 1]))
	// 	largeN_measure_effective(dst);
	// return u;
		// if (u) goto .L_m_e;
		testq	%rsi, %rsi
		jne	.L_m_e
		// if (i != dst->e) goto .L_return;
		cmpq	%rax, largeN_offset_e(%rdi)
		jne	.L_return
		// if (!i) goto .L_return;
		testq	%rax, %rax
		je	.L_return
		// if (dst->le[i - 1]) goto .L_return;
		movq	largeN_offset_le-8(%rdi, %rax, 8), %rax
		testq	%rax, %rax
		jne	.L_return
	.L_m_e_sink:
		// largeN_measure_effective_sink(dst, dst->e);
		pushq	%rsi
		movq	largeN_offset_e(%rdi), %rsi
		callq	largeN_measure_effective_sink
		// return u;
		popq	%rax
		retq
	.p2align 4,,10
	.p2align 4
	.L_m_e:
		// largeN_measure_effective(dst);
		pushq	%rsi
		callq	largeN_measure_effective
		// return u;
		popq	%rax
		retq
	.p2align 4,,10
	.p2align 4
	.L_return:
		// return u;
		movq	%rsi, %rax
		retq
	.size	largeN_u_sub, .-largeN_u_sub
