.file	"inner_cos_4f64.S"

.section	.rodata.cst8, "aM", @progbits, 8
	.align 8
.L_C0:	.double  1.00000000000000000000e+00
.L_C2:	.double -5.00000000000000000000e-01
.L_C4:	.double  4.16666666666666019037e-02
.L_C6:	.double -1.38888888888741095749e-03
.L_C8:	.double  2.48015872894767294178e-05
.L_C10:	.double -2.75573143513906633035e-07
.L_C12:	.double  2.08757232129817482790e-09
.L_C14:	.double -1.13596475577881948265e-11

.text

// void chord_zzz_arch_inner_cos_4f64_load_coff(void)
	.p2align 4
	.globl	chord_zzz_arch_inner_cos_4f64_load_coff
	.hidden	chord_zzz_arch_inner_cos_4f64_load_coff
	.type	chord_zzz_arch_inner_cos_4f64_load_coff, @function
chord_zzz_arch_inner_cos_4f64_load_coff:
	vbroadcastsd	.L_C0(%rip), %ymm8
	vbroadcastsd	.L_C2(%rip), %ymm9
	vbroadcastsd	.L_C4(%rip), %ymm10
	vbroadcastsd	.L_C6(%rip), %ymm11
	vbroadcastsd	.L_C8(%rip), %ymm12
	vbroadcastsd	.L_C10(%rip), %ymm13
	vbroadcastsd	.L_C12(%rip), %ymm14
	vbroadcastsd	.L_C14(%rip), %ymm15
	retq
	.size	chord_zzz_arch_inner_cos_4f64_load_coff, .-chord_zzz_arch_inner_cos_4f64_load_coff

// void chord_zzz_arch_inner_cos_4f64(double *restrict r4, const double *restrict v4)
// rdi: r4(32bytes align)
// rsi: v4(32bytes align) [-pi / 4, pi / 4]
	.p2align 4
	.globl	chord_zzz_arch_inner_cos_4f64
	.hidden	chord_zzz_arch_inner_cos_4f64
	.type	chord_zzz_arch_inner_cos_4f64, @function
chord_zzz_arch_inner_cos_4f64:
	vmovapd	(%rsi), %ymm1
	vmulpd	%ymm1, %ymm1, %ymm2
	vmulpd	%ymm15, %ymm2, %ymm0
	vaddpd	%ymm14, %ymm0, %ymm0
	vmulpd	%ymm2, %ymm0, %ymm0
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
	vmovapd	%ymm0, (%rdi)
	retq
	.size	chord_zzz_arch_inner_cos_4f64, .-chord_zzz_arch_inner_cos_4f64
