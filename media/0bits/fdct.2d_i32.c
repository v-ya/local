#define _DEFAULT_SOURCE
#include "fdct.h"
#include <math.h>

struct media_fdct_2d_i32_s* media_fdct_2d_i32_alloc(uint32_t rank, uint32_t rsh)
{
	struct media_fdct_2d_i32_s *restrict r;
	uintptr_t i, j, p;
	double w, c0, cn;
	if (rank && rsh && rsh <= (sizeof(uint32_t) * 4) &&
		(r = (struct media_fdct_2d_i32_s *) refer_alloz(sizeof(struct media_fdct_2d_i32_s) + sizeof(*r->coefficient) * rank * rank)))
	{
		r->rank = rank;
		r->rsh = rsh;
		p = 0;
		w = M_PI_2 / rank;
		c0 = sqrt(1.0 / rank) * ((uint32_t) 1 << rsh);
		cn = sqrt(2.0 / rank) * ((uint32_t) 1 << rsh);
		for (i = 0; i < rank; ++i)
			r->coefficient[p++] = (int32_t) round(c0);
		for (j = 1; j < rank; ++j)
		{
			for (i = 0; i < rank; ++i)
				r->coefficient[p++] = (int32_t) round(cos((i * 2 + 1) * j * w) * cn);
		}
		return r;
	}
	return NULL;
}

void media_fdct_2d_i32_fdct(const struct media_fdct_2d_i32_s *restrict fdct, int32_t *restrict dst, const int32_t *restrict src)
{
	uintptr_t x, y, u, v;
	uintptr_t rank, rkv, rku, rky;
	int64_t t, mask;
	uint32_t rsh;
	rank = (uintptr_t) fdct->rank;
	rsh = fdct->rsh << 1;
	mask = (int64_t) 1 << (rsh - 1);
	for (v = rky = 0; v < rank; ++v, rkv += rank)
	{
		for (u = rku = 0; u < rank; ++u, rku += rank)
		{
			t = 0;
			for (y = rky = 0; y < rank; ++y, rky += rank)
			{
				for (x = 0; x < rank; ++x)
					t += (int64_t) src[rky + x] * fdct->coefficient[rku + x] * fdct->coefficient[rkv + y];
			}
			dst[rkv + u] = (int32_t) ((t + (t & mask)) >> rsh);
		}
	}
}

void media_fdct_2d_i32_idct(const struct media_fdct_2d_i32_s *restrict fdct, int32_t *restrict dst, const int32_t *restrict src)
{
	uintptr_t x, y, u, v;
	uintptr_t rank, rky, rkv, rku;
	int64_t t, mask;
	uint32_t rsh;
	rank = (uintptr_t) fdct->rank;
	rsh = fdct->rsh << 1;
	mask = (int64_t) 1 << (rsh - 1);
	for (y = rky = 0; y < rank; ++y, rky += rank)
	{
		for (x = 0; x < rank; ++x)
		{
			t = 0;
			for (v = rkv = 0; v < rank; ++v, rkv += rank)
			{
				rkv = v * rank;
				for (u = rku = 0; u < rank; ++u, rku += rank)
					t += (int64_t) src[rkv + u] * fdct->coefficient[rku + x] * fdct->coefficient[rkv + y];
			}
			dst[rky + x] = (int32_t) ((t + (t & mask)) >> rsh);
		}
	}
}
