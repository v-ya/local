#define _DEFAULT_SOURCE
#include "note_details.h"
#include <math.h>

note_details_s* note_details_alloc(uint32_t max)
{
	note_details_s *nd;
	nd = (note_details_s *) refer_alloz(sizeof(note_details_s) + sizeof(note_details_saq_s) * max);
	if (nd) nd->max = max;
	return nd;
}

void note_details_gen(register double *v, uint32_t n, note_details_s *nd, uint32_t l, double a, double q)
{
	double wb, rw, ra, rq;
	register note_details_saq_s *restrict saq;
	register uint32_t i, j;
	if (l)
	{
		wb = M_PI * 2 / l;
		if (l > n) l = n;
		saq = nd->saq;
		j = nd->used;
		while (j)
		{
			rw = wb * j;
			--j;
			ra = saq[j].sa * a;
			rq = saq[j].sq + q;
			for (i = 0; i < l; ++i)
			{
				v[i] += ra * sin(rw * i + rq);
			}
		}
	}
}

void note_details_get(note_details_s *nd, double *v, uint32_t frames)
{
	double s, c, wb, w, wj, vv;
	uint32_t i, j, n;
	if (!nd->used || nd->used > nd->max) nd->used = nd->max;
	n = nd->used;
	wb = M_PI * 2 / frames;
	for (i = 0; i < n; ++i)
	{
		w = wb * (i + 1);
		s = c = 0;
		for (j = 0; j < frames; ++j)
		{
			s += sin(wj = w * j) * (vv = v[j]);
			c += cos(wj) * vv;
		}
		nd->saq[i].sa = sqrt(s * s + c * c) * 2 / frames;
		nd->saq[i].sq = atan2(c, s);
	}
}
