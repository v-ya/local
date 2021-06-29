#include "../x86_64.h"
#include <dylink.h>

void chord_zzz_arch_add_cute_sin_s32(int32_t *restrict p, uintptr_t n, chord_aq_builder_s *restrict aq, intptr_t aq_index)
{
	double b[chord_zzz_block * 3 + 4];
	double *restrict ba;
	double *restrict bq;
	double *restrict br;
	chord_aq_builder_get_f64_f getter;
	uintptr_t m;
	ba = (double *) (((uintptr_t) b + 31) & ~31);
	bq = ba + chord_zzz_block;
	br = bq + chord_zzz_block;
	getter = aq->get64;
	while (n)
	{
		if (n > chord_zzz_block)
			m = chord_zzz_block;
		else m = n;
		getter(aq, m, ba, bq, aq_index);
		chord_zzz_arch_inner_sin_nf64(br, bq, chord_zzz_block, m);
		chord_zzz_arch_inner_vfma_f64s32(p, br, ba, m, (double) chord_zzz_s32_max);
		aq_index += m;
		p += m;
		n -= m;
	}
}

dyl_export(chord_zzz_arch_add_cute_sin_s32, chord_zzz_add_cute_sin_s32);
