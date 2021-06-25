#include "_.h"

void chord_zzz_add_cute_sin_s16(int16_t *restrict p, uintptr_t n, chord_aq_builder_s *restrict aq, intptr_t aq_index)
{
	float a[chord_zzz_block];
	float q[chord_zzz_block];
	chord_aq_builder_get_f32_f getter;
	uintptr_t i, m;
	getter = aq->get32;
	while (n)
	{
		if (n > chord_zzz_block)
			m = chord_zzz_block;
		else m = n;
		getter(aq, m, a, q, aq_index);
		for (i = 0; i < m; ++i)
			p[i] += (int16_t) (a[i] * chord_zzz_s16_max * sinf(q[i]));
		aq_index += m;
		p += m;
		n -= m;
	}
}
