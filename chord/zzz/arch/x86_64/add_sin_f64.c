#include "../x86_64.h"
#include <dylink.h>

void chord_zzz_arch_add_sin_f64(double *restrict p, uintptr_t n, double a, double w, double q)
{
	double b[chord_zzz_sin_block * 2 + 4];
	double *restrict bv;
	double *restrict br;
	uintptr_t m, ii;
	bv = (double *) (((uintptr_t) b + 31) & ~31);
	br = bv + chord_zzz_sin_block;
	ii = 0;
	while (n)
	{
		if (n > chord_zzz_sin_block)
			m = chord_zzz_sin_block;
		else m = n;
		chord_zzz_arch_inner_line_f64(bv, m, ii, w, q);
		chord_zzz_arch_inner_sin_nf64(br, bv, chord_zzz_sin_block, m);
		chord_zzz_arch_inner_cfma_f64(p, br, m, a);
		ii += m;
		p += m;
		n -= m;
	}
}

dyl_export(chord_zzz_arch_add_sin_f64, chord_zzz_add_sin_f64);
