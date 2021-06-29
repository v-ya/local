.file	"inner_clear_avx.S"

.text

// void chord_zzz_arch_inner_clear_avx(void)
	.p2align 4
	.globl	chord_zzz_arch_inner_clear_avx
	.hidden	chord_zzz_arch_inner_clear_avx
	.type	chord_zzz_arch_inner_clear_avx, @function
chord_zzz_arch_inner_clear_avx:
	vzeroupper
	retq
	.size	chord_zzz_arch_inner_clear_avx, .-chord_zzz_arch_inner_clear_avx
