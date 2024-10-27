#include "miko.icpz.h"
#include <exbuffer.h>

struct miko_icpz_w_s {
	exbuffer_t icpz;
};

static void miko_icpz_w_free_func(miko_icpz_w_s *restrict r)
{
	exbuffer_uini(&r->icpz);
}

miko_icpz_w_s* miko_icpz_w_alloc(void)
{
	miko_icpz_w_s *restrict r;
	if ((r = (miko_icpz_w_s *) refer_alloz(sizeof(miko_icpz_w_s))))
	{
		refer_hook_free(r, icpz_w);
		if (exbuffer_init(&r->icpz, 0))
			return r;
		refer_free(r);
	}
	return NULL;
}

miko_icpz_w_s* miko_icpz_w_put(miko_icpz_w_s *restrict r, uintptr_t index)
{
	uintptr_t n;
	uint8_t icpz[((sizeof(index) * 8) + 6) / 7];
	n = 0;
	do {
		icpz[n] = (index & 0x7f) | (!!(index & ~(uintptr_t) 0x7f)) << 7;
		index >>= 7;
		n += 1;
	} while (index);
	if (exbuffer_append(&r->icpz, icpz, n))
		return r;
	return NULL;
}

const uint8_t* miko_icpz_w_data(const miko_icpz_w_s *restrict r, uintptr_t *restrict size)
{
	if (size) *size = r->icpz.used;
	return r->icpz.data;
}
