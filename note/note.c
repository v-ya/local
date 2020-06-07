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

void note_gen(note_s *restrict n, double length, double volume, double basefre, double *v, uint32_t frames, uint32_t sampfre)
{
	double t, dt, q, f, a, apv;
	uint32_t i, j, l, nn, si, sn;
	if (n->base_frequency && n->base_frequency && n->stage_used && n->stage->func)
	{
		t = length;
		nn = (uint32_t) (t * sampfre);
		sn = n->stage_used;
		if (nn)
		{
			i = 0;
			t = q = 0;
			if (nn > frames) nn = frames;
			while (i < nn)
			{
				a = n->envelope(n->envelope_pri, t, volume, length);
				f = n->base_frequency(n->base_frequency_pri, t, basefre, a, a / volume, length);
				dt = sampfre / (f * nn * 2);
				a = n->envelope(n->envelope_pri, t + dt, volume, length);
				f = n->base_frequency(n->base_frequency_pri, t + dt, basefre, a, apv = a / volume, length);
				dt = sampfre / (f * nn * 2);
				t += dt;
				l = (uint32_t) ((q = sampfre / f) + 0.5);
				if ((j = i + l) > nn) break;
				if (l)
				{
					for (si = 0; si < sn; ++si)
						n->stage[si].func(n->stage[si].pri, n->details, t, f, a, apv, length);
					note_details_gen(v + i, frames - i, n->details, l, a, (M_PI * 2) * (q * f / sampfre));
				}
				t += dt;
				i = j + !l;
				q = l - q;
			}
		}
	}
	return ;
}

void note_gen_with_pos(note_s *restrict n, double length, double volume, double basefre, double *v, uint32_t frames, uint32_t sampfre, uint32_t pos)
{
	if (pos < frames)
		note_gen(n, length, volume, basefre, v + pos, frames - pos, sampfre);
}
