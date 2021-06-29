#include "../x86_64.h"
#include <dylink.h>

void chord_zzz_arch_add_cute_sin_f32(float *restrict p, uintptr_t n, chord_aq_builder_s *restrict aq, intptr_t aq_index)
{
	float b[chord_zzz_block * 3 + 8];
	float *restrict ba;
	float *restrict bq;
	float *restrict br;
	chord_aq_builder_get_f32_f getter;
	uintptr_t m;
	ba = (float *) (((uintptr_t) b + 31) & ~31);
	bq = ba + chord_zzz_block;
	br = bq + chord_zzz_block;
	getter = aq->get32;
	while (n)
	{
		if (n > chord_zzz_block)
			m = chord_zzz_block;
		else m = n;
		getter(aq, m, ba, bq, aq_index);
		chord_zzz_arch_inner_sin_nf32(br, bq, chord_zzz_block, m);
		chord_zzz_arch_inner_vfma_f32(p, br, ba, m);
		aq_index += m;
		p += m;
		n -= m;
	}
}

dyl_export(chord_zzz_arch_add_cute_sin_f32, chord_zzz_add_cute_sin_f32);
