.file	"inner_sin_4f64.S"

.section	.rodata.cst8, "aM", @progbits, 8
	.align 8
.L_S1:	.double  1.00000000000000000000e+00
.L_S3:	.double -1.66666666666666324348e-01
.L_S5:	.double  8.33333333332248946124e-03
.L_S7:	.double -1.98412698298579493134e-04
.L_S9:	.double  2.75573137070700676789e-06
.L_S11:	.double -2.50507602534068634195e-08
.L_S13:	.double  1.58969099521155010221e-10

.text

// void chord_zzz_arch_inner_sin_4f64_load_coff(void)
	.p2align 4
	.globl	chord_zzz_arch_inner_sin_4f64_load_coff
	.hidden	chord_zzz_arch_inner_sin_4f64_load_coff
	.type	chord_zzz_arch_inner_sin_4f64_load_coff, @function
chord_zzz_arch_inner_sin_4f64_load_coff:
	vbroadcastsd	.L_S1(%rip), %ymm8
	vbroadcastsd	.L_S3(%rip), %ymm9
	vbroadcastsd	.L_S5(%rip), %ymm10
	vbroadcastsd	.L_S7(%rip), %ymm11
	vbroadcastsd	.L_S9(%rip), %ymm12
	vbroadcastsd	.L_S11(%rip), %ymm13
	vbroadcastsd	.L_S13(%rip), %ymm14
	retq
	.size	chord_zzz_arch_inner_sin_4f64_load_coff, .-chord_zzz_arch_inner_sin_4f64_load_coff

// void chord_zzz_arch_inner_sin_4f64(double *restrict r4, const double *restrict v4)
// rdi: r4(32bytes align)
// rsi: v4(32bytes align) [-pi / 4, pi / 4]
	.p2align 4
	.globl	chord_zzz_arch_inner_sin_4f64
	.hidden	chord_zzz_arch_inner_sin_4f64
	.type	chord_zzz_arch_inner_sin_4f64, @function
chord_zzz_arch_inner_sin_4f64:
	vmovapd	(%rsi), %ymm1
	vmulpd	%ymm1, %ymm1, %ymm2
	vmulpd	%ymm14, %ymm2, %ymm0
	vaddpd	%ymm13, %ymm0, %ymm0
	vmulpd	%ymm2, %ymm0, %ymm0
	vaddpd	%ymm12, %ymm0, %ymm0
	vmulpd	%ymm2, %ymm0, %ymm0
	vaddpd	%ymm11, %ymm0, %ymm0
	vmulpd	%ymm2, %ymm0, %ymm0
	vaddpd	%ymm10, %ymm0, %ymm0
	vmulpd	%ymm2, %ymm0, %ymm0
	vaddpd	%ymm9, %ymm0, %ymm0
	vmulpd	%ymm2, %ymm0, %ymm0
	vaddpd	%ymm8, %ymm0, %ymm0
	vmulpd	%ymm1, %ymm0, %ymm0
	vmovapd	%ymm0, (%rdi)
	retq
	.size	chord_zzz_arch_inner_sin_4f64, .-chord_zzz_arch_inner_sin_4f64
