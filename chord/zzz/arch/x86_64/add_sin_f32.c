#include "../x86_64.h"
#include <dylink.h>

void chord_zzz_arch_add_sin_f32(float *restrict p, uintptr_t n, float a, float w, float q)
{
	float b[chord_zzz_block * 2 + 8];
	float *restrict bv;
	float *restrict br;
	uintptr_t m, ii;
	bv = (float *) (((uintptr_t) b + 31) & ~31);
	br = bv + chord_zzz_block;
	ii = 0;
	while (n)
	{
		if (n > chord_zzz_block)
			m = chord_zzz_block;
		else m = n;
		chord_zzz_arch_inner_line_f32(bv, m, ii, w, q);
		chord_zzz_arch_inner_sin_nf32(br, bv, chord_zzz_block, m);
		chord_zzz_arch_inner_cfma_f32(p, br, m, a);
		ii += m;
		p += m;
		n -= m;
	}
}

dyl_export(chord_zzz_arch_add_sin_f32, chord_zzz_add_sin_f32);
