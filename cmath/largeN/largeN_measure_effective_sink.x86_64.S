.file	"largeN_measure_effective_sink.x86_64.S"
.text

#include "largeN.64.h"

// largeN_s* largeN_measure_effective_sink(largeN_s *restrict n, unit_t last_e)
// rdi: dst
// rsi: last_e
	.p2align 4
	.globl	largeN_measure_effective_sink
	.type	largeN_measure_effective_sink, @function
largeN_measure_effective_sink:
	// rdi: dst
	// rsi: e
	// e = dst->n;
	jmp	.L_check_e
		.p2align 4,,10
		.p2align 4
		.L_loop:
			// if (dst->le[e - 1]) break;
			movq	largeN_offset_le-8(%rdi, %rsi, 8), %rax
			testq	%rax, %rax
			jne	.L_loop_end
			// e -= 1;
			leaq	-1(%rsi), %rsi
			// if (e) continue;
			.L_check_e:
			testq	%rsi, %rsi
			jne	.L_loop
	.L_loop_end:
		movq	%rsi, largeN_offset_e(%rdi)
		movq	%rdi, %rax
		retq
	.size	largeN_measure_effective_sink, .-largeN_measure_effective_sink
