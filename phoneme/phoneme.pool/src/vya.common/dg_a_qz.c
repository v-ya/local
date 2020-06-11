#include <phoneme/phoneme.h>

static dyl_used void dg_a_qz(register note_details_s *restrict d, register double a)
{
	register uint32_t i;
	register note_details_saq_s *saq;
	i = d->used;
	saq = d->saq;
	while (i)
	{
		saq->sa = a;
		saq->sq = 0;
		--i;
		++saq;
	}
}
dyl_export(dg_a_qz, vya.common.dg_a_qz);

