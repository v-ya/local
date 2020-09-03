#include "random_check_pri.h"

static void random_check_free_func(register random_check_s *restrict r)
{
	register random_check_layer_s *restrict layer, *restrict n;
	n = r->layer;
	while ((layer = n))
	{
		n = layer->next;
		refer_free(layer);
	}
	if (r->ml) refer_free(r->ml);
}

random_check_s* random_check_alloc(mlog_s *ml)
{
	register random_check_s *restrict r;
	r = (random_check_s *) refer_alloc(sizeof(random_check_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) random_check_free_func);
		r->layer = NULL;
		r->pnext = &r->layer;
		r->ml = (mlog_s *) refer_save(ml);
	}
	return r;
}

void random_check_check(register random_check_s *restrict rc, register uint32_t random)
{
	register random_check_layer_s *restrict layer;
	layer = rc->layer;
	while (layer)
	{
		layer->check(layer, random);
		layer = layer->next;
	}
}

void random_check_clear(register random_check_s *restrict rc)
{
	register random_check_layer_s *restrict layer;
	layer = rc->layer;
	while (layer)
	{
		layer->clear(layer);
		layer = layer->next;
	}
}

void random_check_dump(register random_check_s *restrict rc)
{
	register random_check_layer_s *restrict layer;
	register mlog_s *ml;
	layer = rc->layer;
	ml = rc->ml;
	while (layer)
	{
		layer->dump(layer, ml);
		layer = layer->next;
	}
}

void random_check_dump_statistics(mlog_s *ml, register const uint32_t *restrict data, register uint32_t number)
{
	register double avg, s2v, min, max, v, k;
	avg = s2v = max = 0;
	min = (double) ~(uint32_t) 0;
	k = 1.0 / number;
	do {
		v = *data++;
		avg += v;
		s2v += v * v;
		if (v > max) max = v;
		if (v < min) min = v;
	} while (--number);
	avg *= k;
	s2v *= k;
	s2v -= avg * avg;
	s2v = (s2v > 0)?sqrt(s2v):0;
	if (avg > 0)
	{
		s2v /= avg;
		max /= avg;
		min /= avg;
		mlog_printf(ml, "s2v: %g, dm: %g (min: %f, max: %f)\n", s2v, max - min, min, max);
	}
}
