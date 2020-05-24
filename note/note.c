#define _DEFAULT_SOURCE
#include "note.h"
#include <math.h>
#include <stdlib.h>

static void note_free_func(note_s *restrict n)
{
	uint32_t i, nn;
	if (n->details) refer_free(n->details);
	if (n->envelope_pri) refer_free(n->envelope_pri);
	if (n->base_frequency_pri) refer_free(n->base_frequency_pri);
	for (i = 0, nn = n->stage_max; i < nn; ++i)
	{
		if (n->stage[i].pri) refer_free(n->stage[i].pri);
	}
}

note_s* note_alloc(uint32_t details_max, uint32_t stage_max)
{
	note_s *n;
	n = NULL;
	if (details_max && stage_max)
	{
		n = refer_alloz(sizeof(note_s) + stage_max * sizeof(note_details_stage_t));
		if (n)
		{
			n->details = note_details_alloc(details_max);
			if (n->details)
			{
				n->stage_max = stage_max;
				refer_set_free(n, (refer_free_f) note_free_func);
				return n;
			}
			refer_free(n);
			n = NULL;
		}
	}
	return n;
}

note_s* note_set_details_max(note_s *restrict n, uint32_t details_max)
{
	if (n->details->max != details_max)
	{
		note_details_s *nd;
		nd = note_details_alloc(details_max);
		if (!nd) return NULL;
		refer_free(n->details);
		n->details = nd;
	}
	return n;
}

void note_set_envelope(note_s *restrict n, note_envelope_f envelope, refer_t pri)
{
	n->envelope = envelope;
	if (n->envelope_pri) refer_free(n->envelope_pri);
	n->envelope_pri = refer_save(pri);
}

void note_set_base_frequency(note_s *restrict n, note_base_frequency_f base_frequency, refer_t pri)
{
	n->base_frequency = base_frequency;
	if (n->base_frequency_pri) refer_free(n->base_frequency_pri);
	n->base_frequency_pri = refer_save(pri);
}

note_s* note_set_stage(note_s *restrict n, uint32_t i, note_details_f func, refer_t pri)
{
	if (i < n->stage_max)
	{
		n->stage[i].func = func;
		if (n->stage[i].pri) refer_free(n->stage[i].pri);
		n->stage[i].pri = refer_save(pri);
		return n;
	}
	return NULL;
}

note_s* note_append_stage(note_s *restrict n, note_details_f func, refer_t pri)
{
	uint32_t i;
	if ((i = n->stage_used) < n->stage_max)
	{
		n->stage[i].func = func;
		if (n->stage[i].pri) refer_free(n->stage[i].pri);
		n->stage[i].pri = refer_save(pri);
		++n->stage_used;
		return n;
	}
	return NULL;
}

void note_clear_stage(note_s *restrict n)
{
	uint32_t i, nn;
	for (i = 0, nn = n->stage_max; i < nn; ++i)
	{
		if (n->stage[i].pri) refer_free(n->stage[i].pri);
		n->stage[i].func = NULL;
		n->stage[i].pri = NULL;
	}
	n->stage_used = 0;
}

void note_random_phase(note_s *restrict n)
{
	static double rand_k = M_PI * 2 / ((double) RAND_MAX + 1);
	note_details_saq_s *saq;
	uint32_t i;
	i = n->details->max;
	saq = n->details->saq;
	while (i)
	{
		saq->sq = rand() * rand_k;
		++saq;
		--i;
	}
}

void note_gen(note_s *restrict n, double t, double volume, double basefre, double *v, uint32_t frames, uint32_t sampfre)
{
	double f, k, a, apv;
	uint32_t i, j, l, nn, si, sn;
	if (n->base_frequency && n->base_frequency && n->stage_used && n->stage->func)
	{
		nn = (uint32_t) (t * sampfre);
		sn = n->stage_used;
		if (nn)
		{
			k = 1.0 / nn;
			i = 0;
			if (nn > frames) nn = frames;
			while (i < nn)
			{
				t = i * k;
				a = n->envelope(n->envelope_pri, t, volume);
				f = n->base_frequency(n->base_frequency_pri, t, basefre, a, a / volume);
				l = sampfre / f;
				t += l * k / 2;
				a = n->envelope(n->envelope_pri, t, volume);
				f = n->base_frequency(n->base_frequency_pri, t, basefre, a, apv = a / volume);
				l = sampfre / f;
				j = i + l;
				if (i > j) goto Err;
				else if (l)
				{
					for (si = 0; si < sn; ++si)
						n->stage[si].func(n->stage[si].pri, n->details, t, f, a, apv);
					note_details_gen_ex(v + i, frames - i, n->details, l, n->envelope, i * k, j * k, volume, n->envelope_pri);
				}
				i = j + 1;
			}
		}
	}
	Err:
	return ;
}

void note_gen_with_pos(note_s *restrict n, double t, double volume, double basefre, double *v, uint32_t frames, uint32_t sampfre, uint32_t pos)
{
	if (pos < frames)
		note_gen(n, t, volume, basefre, v + pos, frames - pos, sampfre);
}
