#define _DEFAULT_SOURCE
#include "audioloop.h"
#include <pcmplay.h>
#include <unistd.h>
#include <stdlib.h>

#define aloop_input_min_data_size  (1u << 16)

static void aloop_input_free_func(aloop_input_s *restrict ai)
{
	if (ai->data) free(ai->data);
}

aloop_input_s* aloop_input_alloc(void)
{
	aloop_input_s *ai;
	ai = (aloop_input_s *) refer_alloz(sizeof(aloop_input_s));
	if (ai) refer_set_free(ai, (refer_free_f) aloop_input_free_func);
	return ai;
}

aloop_input_s* aloop_input_set_frames(register aloop_input_s *restrict ai, uint32_t n)
{
	int32_t *data;
	register uint32_t size;
	if (n <= ai->size)
	{
		label_return:
		return ai;
	}
	size = aloop_input_min_data_size;
	while (size && size < n) size <<= 1;
	if (!size) goto label_error;
	data = (int32_t *) malloc((size_t) size << 2);
	if (!data) goto label_error;
	if (ai->data) free(ai->data);
	ai->data = data;
	ai->size = size;
	goto label_return;
	label_error:
	return NULL;
}

static void aloop_input_fq2s32(int32_t *restrict r, double *restrict *restrict v, uint32_t channels, uint32_t frames)
{
	static const uint32_t smax = 0x7fffffffu;
	register int32_t *restrict d;
	register double *restrict s;
	register uint32_t n, p;
	uint32_t m;
	m = frames >> 4;
	frames -= m << 4;
	p = channels;
	while (channels)
	{
		s = *v;
		d = r;
		n = m;
		while (n)
		{
			--n;
			*d = (int32_t) (*s++ * smax);
			d += p;
			*d = (int32_t) (*s++ * smax);
			d += p;
			*d = (int32_t) (*s++ * smax);
			d += p;
			*d = (int32_t) (*s++ * smax);
			d += p;
			*d = (int32_t) (*s++ * smax);
			d += p;
			*d = (int32_t) (*s++ * smax);
			d += p;
			*d = (int32_t) (*s++ * smax);
			d += p;
			*d = (int32_t) (*s++ * smax);
			d += p;
			*d = (int32_t) (*s++ * smax);
			d += p;
			*d = (int32_t) (*s++ * smax);
			d += p;
			*d = (int32_t) (*s++ * smax);
			d += p;
			*d = (int32_t) (*s++ * smax);
			d += p;
			*d = (int32_t) (*s++ * smax);
			d += p;
			*d = (int32_t) (*s++ * smax);
			d += p;
			*d = (int32_t) (*s++ * smax);
			d += p;
			*d = (int32_t) (*s++ * smax);
			d += p;
		}
		n = frames;
		while (n)
		{
			--n;
			*d = (int32_t) (*s++ * smax);
			d += p;
		}
		--channels;
		++r;
		++v;
	}
}

aloop_input_s* aloop_input_set_double(register aloop_input_s *restrict ai, double *restrict *restrict v, uint32_t channels, uint32_t sampfre, uint32_t frames)
{
	if (channels && sampfre && aloop_input_set_frames(ai, channels * frames))
	{
		aloop_input_fq2s32(ai->data, v, channels, frames);
		ai->channels = channels;
		ai->sampfre = sampfre;
		ai->frames = frames;
		return ai;
	}
	return NULL;
}

static void aloop_free_func(aloop_s *restrict r)
{
	r->status = 0;
	if (r->thread) pthread_join(r->thread, NULL);
	if (r->player) refer_free(r->player);
	if (r->next) refer_free(r->next);
	if (r->ai1) refer_free(r->ai1);
	if (r->ai2) refer_free(r->ai2);
	if (r->ai3) refer_free(r->ai3);
}

static void aloop_push_input(register aloop_s *a, register aloop_input_s *restrict ai)
{
	label_loop:
	if (!__sync_val_compare_and_swap(&a->ai1, NULL, ai)) goto label_return;
	if (!__sync_val_compare_and_swap(&a->ai2, NULL, ai)) goto label_return;
	if (!__sync_val_compare_and_swap(&a->ai3, NULL, ai)) goto label_return;
	__asm__ ("rep; nop;");
	goto label_loop;
	label_return:
	return ;
}

static aloop_input_s* aloop_pull_input(register aloop_s *a)
{
	register aloop_input_s *restrict ai;
	label_loop:
	if ((ai = __sync_lock_test_and_set(&a->ai3, NULL))) goto label_return;
	if ((ai = __sync_lock_test_and_set(&a->ai2, NULL))) goto label_return;
	if ((ai = __sync_lock_test_and_set(&a->ai1, NULL))) goto label_return;
	__asm__ ("rep; nop;");
	goto label_loop;
	label_return:
	return ai;
}

static aloop_input_s* aloop_get_next(register aloop_s *a, register aloop_input_s *restrict play)
{
	register aloop_input_s *restrict next;
	next = __sync_lock_test_and_set(&a->next, NULL);
	if (next)
	{
		aloop_push_input(a, play);
		play = next;
	}
	return play;
}

static void aloop_set_next(register aloop_s *a, register aloop_input_s *restrict next)
{
	next = __sync_lock_test_and_set(&a->next, next);
	if (next) aloop_push_input(a, next);
}

static void aloop_play_input(register aloop_s *a, aloop_input_s *restrict play)
{
	void *data;
	uint32_t frames;
	data = play->data;
	frames = play->frames;
	while (a->status && frames)
	{
		if (pcmplay_write_once(a->player, &data, &frames) < 0)
			break;
	}
}

static void aloop_wait_player(register aloop_s *a)
{
	while (a->status && pcmplay_get_avail(a->player) > 0)
		usleep(a->usleep_time);
}

static void* aloop_thread_func(register aloop_s *a)
{
	aloop_input_s *restrict play;
	play = NULL;
	while (a->status)
	{
		if (a->next) play = aloop_get_next(a, play);
		if (!play) goto label_wait;
		if (!a->player || a->channels != play->channels || a->sampfre != play->sampfre)
		{
			a->channels = play->channels;
			a->sampfre = play->sampfre;
			if (a->player)
			{
				refer_free(a->player);
				a->player = NULL;
			}
			if (play->channels && play->sampfre)
				a->player = pcmplay_alloc(play->channels, play->sampfre, pcmplay_format_s32);
			if (!a->player)
			{
				label_wait:
				usleep(a->usleep_time);
				continue;
			}
		}
		if (play->frames && !pcmplay_prepare(a->player))
		{
			aloop_play_input(a, play);
			aloop_wait_player(a);
		}
	}
	aloop_push_input(a, play);
	return NULL;
}

aloop_s* aloop_alloc(uint32_t usleep_time)
{
	register aloop_s *restrict r;
	r = (aloop_s *) refer_alloz(sizeof(aloop_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) aloop_free_func);
		r->ai1 = aloop_input_alloc();
		r->ai2 = aloop_input_alloc();
		r->ai3 = aloop_input_alloc();
		if (r->ai1 && r->ai2 && r->ai3)
		{
			r->status = 1;
			r->usleep_time = usleep_time;
			if (!pthread_create(&r->thread, NULL, (void *(*)(void *)) aloop_thread_func, r))
				return r;
		}
		refer_free(r);
	}
	return NULL;
}

aloop_s* aloop_update_double(aloop_s *restrict r, double *restrict *restrict v, uint32_t channels, uint32_t sampfre, uint32_t frames)
{
	if (channels && sampfre && frames)
	{
		register aloop_input_s *restrict ai;
		ai = aloop_pull_input(r);
		if (aloop_input_set_double(ai, v, channels, sampfre, frames))
		{
			aloop_set_next(r, ai);
			return r;
		}
		aloop_push_input(r, ai);
	}
	return NULL;
}

void aloop_uini(void)
{
	pcmplay_uini();
}
