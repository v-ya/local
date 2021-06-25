.file	"inner_cos_8f32.S"

.section	.rodata.cst4, "aM", @progbits, 4
	.align 4
.L_C0:	.float  1.00000000000000000000e+00
.L_C2:	.float -5.00000000000000000000e-01
.L_C4:	.float  4.16666666666666666667e-02
.L_C6:	.float -1.38888888888888888888e-03
.L_C8:	.float  2.48015873015873015873e-05
.L_C10:	.float -2.75573192239858906526e-07

.text

// void chord_zzz_arch_inner_cos_8f32_load_coff(void)
	.p2align 4
	.globl	chord_zzz_arch_inner_cos_8f32_load_coff
	.hidden	chord_zzz_arch_inner_cos_8f32_load_coff
	.type	chord_zzz_arch_inner_cos_8f32_load_coff, @function
chord_zzz_arch_inner_cos_8f32_load_coff:
	vbroadcastss	.L_C0(%rip), %ymm8
	vbroadcastss	.L_C2(%rip), %ymm9
	vbroadcastss	.L_C4(%rip), %ymm10
	vbroadcastss	.L_C6(%rip), %ymm11
	vbroadcastss	.L_C8(%rip), %ymm12
	vbroadcastss	.L_C10(%rip), %ymm13
	retq
	.size	chord_zzz_arch_inner_cos_8f32_load_coff, .-chord_zzz_arch_inner_cos_8f32_load_coff

// void chord_zzz_arch_inner_cos_8f32(float *restrict r4, const float *restrict v4)
// rdi: r4(32bytes align)
// rsi: v4(32bytes align) [-pi / 4, pi / 4]
	.p2align 4
	.globl	chord_zzz_arch_inner_cos_8f32
	.hidden	chord_zzz_arch_inner_cos_8f32
	.type	chord_zzz_arch_inner_cos_8f32, @function
chord_zzz_arch_inner_cos_8f32:
	vmovaps	(%rsi), %ymm1
	vmulps	%ymm1, %ymm1, %ymm2
	vmulps	%ymm13, %ymm2, %ymm0
	vaddps	%ymm12, %ymm0, %ymm0
	vmulps	%ymm2, %ymm0, %ymm0
	vaddps	%ymm11, %ymm0, %ymm0
	vmulps	%ymm2, %ymm0, %ymm0
	vaddps	%ymm10, %ymm0, %ymm0
	vmulps	%ymm2, %ymm0, %ymm0
	vaddps	%ymm9, %ymm0, %ymm0
	vmulps	%ymm2, %ymm0, %ymm0
	vaddps	%ymm8, %ymm0, %ymm0
	vmovaps	%ymm0, (%rdi)
	retq
	.size	chord_zzz_arch_inner_cos_8f32, .-chord_zzz_arch_inner_cos_8f32
