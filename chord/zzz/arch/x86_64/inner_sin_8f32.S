.file	"inner_sin_8f32.S"

.section	.rodata.cst4, "aM", @progbits, 4
	.align 4
.L_S1:	.float  1.00000000000000000000e+00
.L_S3:	.float -1.66666666666666666667e-01
.L_S5:	.float  8.33333333333333333333e-03
.L_S7:	.float -1.98412698412698412698e-04
.L_S9:	.float  2.75573192239858906526e-06

.text

// void chord_zzz_arch_inner_sin_8f32_load_coff(void)
	.p2align 4
	.globl	chord_zzz_arch_inner_sin_8f32_load_coff
	.hidden	chord_zzz_arch_inner_sin_8f32_load_coff
	.type	chord_zzz_arch_inner_sin_8f32_load_coff, @function
chord_zzz_arch_inner_sin_8f32_load_coff:
	vbroadcastss	.L_S1(%rip), %ymm8
	vbroadcastss	.L_S3(%rip), %ymm9
	vbroadcastss	.L_S5(%rip), %ymm10
	vbroadcastss	.L_S7(%rip), %ymm11
	vbroadcastss	.L_S9(%rip), %ymm12
	retq
	.size	chord_zzz_arch_inner_sin_8f32_load_coff, .-chord_zzz_arch_inner_sin_8f32_load_coff

// void chord_zzz_arch_inner_sin_8f32(float *restrict r4, const float *restrict v4)
// rdi: r4(32bytes align)
// rsi: v4(32bytes align) [-pi / 4, pi / 4]
	.p2align 4
	.globl	chord_zzz_arch_inner_sin_8f32
	.hidden	chord_zzz_arch_inner_sin_8f32
	.type	chord_zzz_arch_inner_sin_8f32, @function
chord_zzz_arch_inner_sin_8f32:
	vmovaps	(%rsi), %ymm1
	vmulps	%ymm1, %ymm1, %ymm2
	vmulps	%ymm12, %ymm2, %ymm0
	vaddps	%ymm11, %ymm0, %ymm0
	vmulps	%ymm2, %ymm0, %ymm0
	vaddps	%ymm10, %ymm0, %ymm0
	vmulps	%ymm2, %ymm0, %ymm0
	vaddps	%ymm9, %ymm0, %ymm0
	vmulps	%ymm2, %ymm0, %ymm0
	vaddps	%ymm8, %ymm0, %ymm0
	vmulps	%ymm1, %ymm0, %ymm0
	vmovaps	%ymm0, (%rdi)
	retq
	.size	chord_zzz_arch_inner_sin_8f32, .-chord_zzz_arch_inner_sin_8f32
