.file	"image_resample_get_dst.x86_64.S"
.text

#define offset_src        0x00
#define offset_dst        0x08
#define offset_multicalc  0x10
#define offset_d_size     0x18
#define offset_s_width    0x20
#define offset_s_height   0x24
#define offset_d_width    0x28
#define offset_d_height   0x2c
#define offset_matrix     0x30
#define offset_bgcolor    0x54
#define offset_n_mc       0x58
#define offset_n_block    0x5c

// 内部缓存行对齐
// .p2align 4,,10
// .p2align 3

// rdi:   dst
// rsi:   src
// edx:   x
// rcx:   n
// rbx:   matrix
// r8d:   d_width
// r9d:   y
// r10d:  s_width
// r11d:  s_height
// r12d:  bgcolor
// r13:   inter used
// r14:   inter used
// r15:   inter used
// (rsp): d_height

// image_resample_s* image_resample_get_dst_subblock(image_resample_s *restrict r, uint32_t x, uint32_t y, uintptr_t n)
// rdi: r
// esi: x
// edx: y
// rcx: n
	.p2align 4
	.globl	image_resample_get_dst_subblock
	.type	image_resample_get_dst_subblock, @function
image_resample_get_dst_subblock:
	// load register
	pushq	%rbx
	pushq	%r12
	pushq	%r13
	pushq	%r14
	pushq	%r15
	pushq	%rdi
	movl	%edx, %r9d
	movl	%esi, %edx
	movl	offset_bgcolor(%rdi), %r12d
	movl	offset_s_height(%rdi), %r11d
	movl	offset_s_width(%rdi), %r10d
	movl	offset_d_height(%rdi), %eax
	pushq	%rax
	movl	offset_d_width(%rdi), %r8d
	leaq	offset_matrix(%rdi), %rbx
	movq	offset_src(%rdi), %rsi
	movq	offset_dst(%rdi), %rdi
	// check dst && src && n;
	testq	%rdi, %rdi
	je	.L_fail
	testq	%rsi, %rsi
	je	.L_fail
	testq	%rcx, %rcx
	je	.L_fail
	// !d_width || !d_height; goto ok
	testl	%r8d, %r8d
	je	.L_ok
	testl	%eax, %eax
	je	.L_ok
	// dst += y * d_width + x
	movl	%r9d, %eax
	imull	%r8d, %eax
	addl	%edx, %eax
	leaq	(%rdi, %rax, 4), %rdi
	// xmm12: 1
	// xmm13: 0
	// xmm14: s_width
	// xmm15: s_height
	movss		.L_const_float_1(%rip), %xmm12
	pxor		%xmm13, %xmm13
	cvtsi2ssl	%r10d, %xmm14
	cvtsi2ssl	%r11d, %xmm15
	jmp	.L_loop_entry
	// cache line align
	.p2align 4,,10
	.p2align 4
	// loop
		.L_loop:
			// calc x, y, z (xmm0 - xmm2)
			// xmm8:  m[1] * y + m[2]
			// xmm9:  m[4] * y + m[5]
			// xmm10: m[7] * y + m[8]
			cvtsi2ssl	%edx, %xmm0
			movaps		%xmm0, %xmm1
			movaps		%xmm0, %xmm2
			mulss		0(%rbx), %xmm0
			mulss		12(%rbx), %xmm1
			mulss		24(%rbx), %xmm2
			addss		%xmm8, %xmm0
			addss		%xmm9, %xmm1
			addss		%xmm10, %xmm2
			rcpss		%xmm2, %xmm2
			mulss		%xmm2, %xmm0
			mulss		%xmm2, %xmm1
			// x: xmm0 r14d, y: xmm1 r15d
			// get src pixel (%eax)
				// check x, y in [(0, 0) ~ (sw, sh))
				movl	%r12d, %eax
				comiss	%xmm0, %xmm13
				ja	.L_get_src_end
				comiss	%xmm0, %xmm14
				jbe	.L_get_src_end
				comiss	%xmm1, %xmm13
				ja	.L_get_src_end
				comiss	%xmm1, %xmm15
				jbe	.L_get_src_end
				// xmm0: mod(x, 1)
				// xmm1: mod(y, 1)
				// xmm2: 1 - xmm0
				// xmm3: 1 - xmm1
				cvttss2sil	%xmm0, %r14d
				cvttss2sil	%xmm1, %r15d
				cvtsi2ssl	%r14d, %xmm2
				subss		%xmm2, %xmm0
				cvtsi2ssl	%r15d, %xmm2
				subss		%xmm2, %xmm1
				movaps		%xmm12, %xmm2
				movaps		%xmm12, %xmm3
				subss		%xmm0, %xmm2
				subss		%xmm1, %xmm3
				shufps		$0, %xmm0, %xmm0
				shufps		$0, %xmm1, %xmm1
				shufps		$0, %xmm2, %xmm2
				shufps		$0, %xmm3, %xmm3
				// rax:  p
				// xmm4: BGRA
				// xmm5: s
				// xmm6: k
				// xmm7: -
				// rax = src + ((y * s_width + x) << 2)
				movl	%r15d, %eax
				imull	%r10d, %eax
				addl	%r14d, %eax
				shlq	$4, %rax
				leaq	(%rsi, %rax, 1), %rax
				// k = 2 - xmm0 - xmm1
				movaps	%xmm2, %xmm6
				addps	%xmm3, %xmm6
				movaps	%xmm6, %xmm5
				// bgra =
				movaps	(%rax), %xmm4
				mulps	%xmm6, %xmm4
				// test x + 1 < s_width
				addl	$1, %r14d
				cmpl	%r14d, %r10d
				jbe	.L_only_y_plus
					// p += 4
					leaq	16(%rax), %rax
					// k = xmm0 + 1 - xmm1
					movaps	%xmm0, %xmm6
					addps	%xmm3, %xmm6
					addps	%xmm6, %xmm5
					// bgra +=
					movaps	(%rax), %xmm7
					mulps	%xmm6, %xmm7
					addps	%xmm7, %xmm4
					// test y + 1 < s_height
					addl	$1, %r15d
					cmpl	%r15d, %r11d
					jbe	.L_calc_color
						// p += r->s_width * 4
						movq	%r10, %r13
						shlq	$4, %r13
						addq	%r13, %rax
						// k = xmm0 + xmm1
						movaps	%xmm0, %xmm6
						addps	%xmm1, %xmm6
						addps	%xmm6, %xmm5
						// bgra +=
						movaps	(%rax), %xmm7
						mulps	%xmm6, %xmm7
						addps	%xmm7, %xmm4
						// p -= 4
						leaq	-16(%rax), %rax
						.L_only_y_plus_entry:
						// k = xmm1 + 1 - xmm0
						movaps	%xmm1, %xmm6
						addps	%xmm2, %xmm6
						addps	%xmm6, %xmm5
						// bgra +=
						movaps	(%rax), %xmm7
						mulps	%xmm6, %xmm7
						addps	%xmm7, %xmm4
				.L_calc_color:
				// check s
				ucomiss	%xmm5, %xmm13
				je	.L_get_src_end
				// bgra /= s
				rcpps	%xmm5, %xmm5
				mulps	%xmm5, %xmm4
				// bgra set
				movaps	%xmm4, %xmm5
				// now:
				// xmm4: [B, G, R, A]
				// xmm5: [B, G, R, A]
				shufps	$0b01001110, %xmm4, %xmm5
				// now:
				// xmm4: [B, G, R, A]
				// xmm5: [R, A, B, G]
				cvtps2pi %xmm4, %mm4
				cvtps2pi %xmm5, %mm5
				movq	%mm5, %rax
				shl	$16, %rax
				movq	%mm4, %r13
				orq	%r13, %rax
				movq	%rax, %r13
				shrq	$24, %rax
				orq	%r13, %rax
			.L_get_src_end:
			// set
			movl	%eax, (%rdi)
			leaq	4(%rdi), %rdi
			// check n
			subq	$1, %rcx
			testq	%rcx, %rcx
			je	.L_ok
			// x += 1
			addl	$1, %edx
			cmpl	%edx, %r8d
			ja	.L_loop
		// y(%r9d) += 1, x = 0
		addl	$1, %r9d
		xorl	%edx, %edx
		.L_loop_entry:
		// load xmm8, xmm9, xmm10
		cvtsi2ssl	%r9d, %xmm8
		movaps		%xmm8, %xmm9
		movaps		%xmm8, %xmm10
		mulss		4(%rbx), %xmm8
		addss		8(%rbx), %xmm8
		mulss		16(%rbx), %xmm9
		addss		20(%rbx), %xmm9
		mulss		28(%rbx), %xmm10
		addss		32(%rbx), %xmm10
		cmpl	%r9d, (%rsp)
		ja	.L_loop
	.L_ok:
	addq	$8, %rsp
	popq	%rax
	.L_return:
	popq	%r15
	popq	%r14
	popq	%r13
	popq	%r12
	popq	%rbx
	retq
	.L_fail:
	addq	$16, %rsp
	xorl	%eax, %eax
	jmp	.L_return
	.L_only_y_plus:
	// test y + 1 < s_height
	addl	$1, %r15d
	cmpl	%r15d, %r11d
	jbe	.L_calc_color
	// p += r->s_width * 4
	movq	%r10, %r13
	shlq	$4, %r13
	addq	%r13, %rax
	jmp	.L_only_y_plus_entry
	.size	image_resample_get_dst_subblock, .-image_resample_get_dst_subblock

.section	.rodata.cst4,"aM",@progbits,4

	.align 4
.L_const_float_1:
	.long	0x3f800000
