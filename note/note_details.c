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

void note_details_gen(double *v, uint32_t n, note_details_s *nd, uint32_t l, double a)
{
	double wb, c;
	note_details_saq_s *saq;
	uint32_t i, j;
	if (l)
	{
		wb = M_PI * 2 / l;
		if (l < n) ++l;
		else l = n;
		n = nd->used;
		saq = nd->saq;
		for (i = 0; i < l; ++i)
		{
			c = 0;
			for (j = 0; j < n; ++j)
			{
				c += saq[j].sa * sin(wb * (j + 1) * i + saq[j].sq);
			}
			v[i] += a * c;
		}
	}
}

void note_details_gen_ex(double *v, uint32_t n, note_details_s *nd, uint32_t l, note_details_a_f af, double a1, double a2, refer_t pri)
{
	double wb, c;
	note_details_saq_s *saq;
	uint32_t i, j;
	if (l)
	{
		wb = M_PI * 2 / l;
		a2 = (a2 - a1) / l;
		if (l < n) ++l;
		else l = n;
		n = nd->used;
		saq = nd->saq;
		for (i = 0; i < l; ++i)
		{
			c = 0;
			for (j = 0; j < n; ++j)
			{
				c += saq[j].sa * sin(wb * (j + 1) * i + saq[j].sq);
			}
			v[i] += af(a1 + a2 * i, pri) * c;
		}
	}
}
