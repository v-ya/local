#define _DEFAULT_SOURCE
#include "note.h"
#include <math.h>
#include <stdlib.h>

static void note_free_func(note_s *restrict n)
{
	if (n->envelope_pri) refer_free(n->envelope_pri);
	if (n->base_frequency_pri) refer_free(n->base_frequency_pri);
	if (n->phoneme_pri) refer_free(n->phoneme_pri);
	if (n->tone_pri) refer_free(n->tone_pri);
	if (n->details) refer_free(n->details);
}

note_s* note_alloc(uint32_t details_max)
{
	note_s *n;
	n = refer_alloz(sizeof(note_s));
	if (n)
	{
		n->details = note_details_alloc(details_max);
		if (n->details)
		{
			refer_set_free(n, (refer_free_f) note_free_func);
			return n;
		}
		refer_free(n);
		n = NULL;
	}
	return n;
}

void note_set_envelope(note_s *n, note_envelope_f envelope, refer_t pri)
{
	n->envelope = envelope;
	if (n->envelope_pri) refer_free(n->envelope_pri);
	n->envelope_pri = refer_save(pri);
}

void note_set_base_frequency(note_s *n, note_base_frequency_f base_frequency, refer_t pri)
{
	n->base_frequency = base_frequency;
	if (n->base_frequency_pri) refer_free(n->base_frequency_pri);
	n->base_frequency_pri = refer_save(pri);
}

void note_set_phoneme(note_s *n, note_details_f phoneme, refer_t pri)
{
	n->phoneme = phoneme;
	if (n->phoneme_pri) refer_free(n->phoneme_pri);
	n->phoneme_pri = refer_save(pri);
}

void note_set_tone(note_s *n, note_details_f tone, refer_t pri)
{
	n->tone = tone;
	if (n->tone_pri) refer_free(n->tone_pri);
	n->tone_pri = refer_save(pri);
}

void note_random_phase(note_s *n)
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

void note_gen(note_s *n, double t, double *v, uint32_t frames, uint32_t sampfre)
{
	double w, k;
	uint32_t i, j, l, nn;
	if (n->base_frequency && n->base_frequency && n->tone)
	{
		nn = (uint32_t) (t * sampfre);
		if (nn)
		{
			k = 1.0 / nn;
			i = 0;
			if (nn > frames) nn = frames;
			while (i < nn)
			{
				t = i * k;
				w = n->base_frequency(t, n->envelope(t, n->envelope_pri), n->base_frequency_pri);
				l = (M_PI * 2) / w * sampfre;
				t += l * k / 2;
				w = n->base_frequency(t, n->envelope(t, n->envelope_pri), n->base_frequency_pri);
				l = (M_PI * 2) / w * sampfre;
				j = i + l;
				if (i > j) goto Err;
				else if (l)
				{
					n->tone(n->details, w, n->tone_pri);
					if (n->phoneme) n->phoneme(n->details, w, n->phoneme_pri);
					note_details_gen_ex(v + i, frames - i, n->details, l, n->envelope, i * k, j * k, n->envelope_pri);
				}
				i = j + 1;
			}
		}
	}
	Err:
	return ;
}

void note_gen_with_pos(note_s *n, double t, double *v, uint32_t frames, uint32_t sampfre, uint32_t pos)
{
	if (pos < frames)
		note_gen(n, t, v + pos, frames - pos, sampfre);
}
