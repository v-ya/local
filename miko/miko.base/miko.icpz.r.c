#include "miko.icpz.h"

miko_icpz_r_t* miko_icpz_r_initial(miko_icpz_r_t *restrict r, const uint8_t *restrict icpz, uintptr_t size)
{
	r->icpz = icpz;
	r->size = size;
	r->pos = 0;
	return icpz?r:NULL;
}

miko_icpz_r_t* miko_icpz_r_get(miko_icpz_r_t *restrict r, uintptr_t *restrict index)
{
	const uint8_t *restrict icpz;
	uintptr_t p, n, v;
	uint32_t nbits;
	if ((p = r->pos) < (n = r->size))
	{
		icpz = r->icpz;
		v = 0;
		nbits = 0;
		do {
			v |= (uintptr_t) (icpz[p] & 0x7f) << nbits;
			nbits += 7;
			if (!(icpz[p++] & 0x80))
			{
				r->pos = p;
				if (index) *index = v;
				return r;
			}
		} while (p < n && nbits < (sizeof(v) * 8));
		r->pos = p;
	}
	return NULL;
}
