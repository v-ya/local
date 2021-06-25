#include "_.h"

void chord_zzz_add_cute_sin_f64(double *restrict p, uintptr_t n, chord_aq_builder_s *restrict aq, intptr_t aq_index)
{
	double a[chord_zzz_block];
	double q[chord_zzz_block];
	chord_aq_builder_get_f64_f getter;
	uintptr_t i, m;
	getter = aq->get64;
	while (n)
	{
		if (n > chord_zzz_block)
			m = chord_zzz_block;
		else m = n;
		getter(aq, m, a, q, aq_index);
		for (i = 0; i < m; ++i)
			p[i] += a[i] * sin(q[i]);
		aq_index += m;
		p += m;
		n -= m;
	}
}
