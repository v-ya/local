#define _DEFAULT_SOURCE
#include "phoneme_output.h"
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/resource.h>

static phoneme_buffer_t* phoneme_buffer_init(phoneme_buffer_t *restrict b, uint32_t size)
{
	b->buffer = (double *) calloc(b->size = size, sizeof(double));
	if (b->buffer) return b;
	b->size = 0;
	return NULL;
}

static void phoneme_buffer_uini(phoneme_buffer_t *restrict b)
{
	if (b->buffer) free(b->buffer);
}

static phoneme_buffer_t* phoneme_buffer_set_frames(phoneme_buffer_t *restrict pb, uint32_t frames)
{
	double *buffer;
	uint32_t size;
	if (frames > pb->size)
	{
		size = (uint32_t) pb->size;
		while (size < 0x10000000u)
		{
			size <<= 1;
			if (frames <= size)
				goto label_resize;
		}
		Err:
		return NULL;
		label_resize:
		buffer = (double *) realloc(pb->buffer, size * sizeof(double));
		if (!buffer) goto Err;
		memset((pb->buffer = buffer) + pb->size, 0, (size - pb->size) * sizeof(double));
		pb->size = size;
	}
	return pb;
}

static void* phoneme_output_core_thread_func(register phoneme_output_core_t *restrict core)
{
	core->tid = syscall(SYS_gettid);
	while (core->running)
	{
		if (core->busy)
		{
			if (core->note && core->sampfre && phoneme_buffer_set_frames(&core->buffer, core->length))
			{
				note_gen(core->note, core->length, core->volume, core->basefre, core->buffer.buffer, core->length, core->sampfre);
			}
			else core->sampfre = core->length = 0;
			core->busy = 0;
		}
		else usleep(core->usleep_time);
	}
	return NULL;
}

static phoneme_output_core_t* phoneme_output_core_init(phoneme_output_core_t *restrict core, uint32_t size, uint32_t usleep_time)
{
	if (phoneme_buffer_init(&core->buffer, size))
	{
		core->note = NULL;
		core->running = 1;
		core->busy = 0;
		core->usleep_time = usleep_time;
		if (!pthread_create(&core->thread, NULL, (void*(*)(void*)) phoneme_output_core_thread_func, core))
		{
			return core;
		}
		phoneme_buffer_uini(&core->buffer);
	}
	core->thread = 0;
	return NULL;
}

static void phoneme_output_core_uini(phoneme_output_core_t *restrict core)
{
	if (core->thread)
	{
		core->running = 0;
		pthread_join(core->thread, NULL);
	}
	phoneme_buffer_uini(&core->buffer);
	if (core->note) refer_free(core->note);
}

static phoneme_output_core_t* phoneme_output_core_call(phoneme_output_core_t *restrict core, note_s *restrict note, uint32_t sampfre, double pos, double length, double volume, double basefre)
{
	if (!core->busy && (note = note_dump(note)))
	{
		if (core->note) refer_free(core->note);
		core->note = note;
		core->volume = volume;
		core->basefre = basefre;
		core->length = (uint32_t) (length * sampfre);
		core->pos = (uint32_t) (pos * sampfre);
		core->sampfre = sampfre;
		core->busy = 1;
		return core;
	}
	return NULL;
}

static phoneme_buffer_t* phoneme_output_core_join(phoneme_output_core_t *restrict core, phoneme_buffer_t *restrict pb)
{
	while (core->busy) usleep(core->usleep_time);
	if (core->note)
	{
		refer_free(core->note);
		core->note = NULL;
		if (!core->sampfre) goto Err;
		if (core->length)
		{
			double *restrict d, *restrict s;
			register uint32_t n;
			n = core->length;
			d = pb->buffer + core->pos;
			s = core->buffer.buffer;
			while (n)
			{
				*d++ += *s++;
				--n;
			}
		}
	}
	if (core->length)
	{
		memset(core->buffer.buffer, 0, core->length * sizeof(double));
		core->length = 0;
	}
	return pb;
	Err:
	return NULL;
}

static void phoneme_output_free_func(phoneme_output_s *restrict po)
{
	uint32_t i, n;
	phoneme_buffer_uini(&po->output);
	for (i = 0, n = po->core_number; i < n; ++i)
	{
		phoneme_output_core_uini(po->core + i);
	}
}

phoneme_output_s* phoneme_output_alloc(uint32_t sampfre, uint32_t frames, uint32_t core_number, uint32_t usleep_time)
{
	phoneme_output_s *r;
	r = refer_alloz(sizeof(phoneme_output_s) + core_number * sizeof(phoneme_output_core_t));
	if (r)
	{
		if (!frames) frames = (16 << 10);
		r->sampfre = sampfre;
		r->core_number = core_number;
		if (phoneme_buffer_init(&r->output, frames))
		{
			refer_set_free(r, (refer_free_f) phoneme_output_free_func);
			for (sampfre = 0; sampfre < core_number; ++sampfre)
			{
				if (!phoneme_output_core_init(r->core + sampfre, frames, usleep_time))
					goto Err;
			}
			return r;
		}
		Err:
		refer_free(r);
	}
	return NULL;
}

phoneme_output_s* phoneme_output_set_priority(phoneme_output_s *restrict po, double nice)
{
	phoneme_output_s *r;
	r = NULL;
	if (po->core_number)
	{
		phoneme_output_core_t *core;
		uint32_t i;
		int _ni;
		r = po;
		if (nice < 0) _ni = (int) (nice * (-PRIO_MIN) - 0.5);
		else _ni = (int) (nice * PRIO_MAX + 0.5);
		for (i = 0, core = po->core; i < po->core_number; ++i, ++core)
		{
			while (!*(volatile uintptr_t*) &core->tid)
				usleep(core->usleep_time);
			if (setpriority(PRIO_PROCESS, (id_t) core->tid, _ni))
				r = NULL;
		}
	}
	return r;
}

phoneme_output_s* phoneme_output_set_sampfre(phoneme_output_s *restrict po, uint32_t sampfre)
{
	po->sampfre = sampfre;
	return po;
}

phoneme_output_s* phoneme_output_set_frames(phoneme_output_s *restrict po, double t)
{
	uint32_t frames;
	frames = (uint32_t) (t * po->sampfre);
	if (frames > po->frames)
	{
		if (frames > po->output.size && !phoneme_buffer_set_frames(&po->output, frames))
			return NULL;
		po->frames = frames;
	}
	return po;
}

phoneme_output_s* phoneme_output_gen_note(phoneme_output_s *restrict po, note_s *restrict note, double pos, double length, double volume, double basefre)
{
	if (!po->sampfre) goto End;
	if (phoneme_output_set_frames(po, pos + length))
	{
		if (po->core_number)
		{
			if (!phoneme_output_core_join(po->core + po->core_index, &po->output))
				goto Err;
			if (!phoneme_output_core_call(po->core + po->core_index, note, po->sampfre, pos, length, volume, basefre))
				goto Err;
			po->core_index = (po->core_index + 1) % po->core_number;
		}
		else
		{
			note_gen_with_pos(note, length, volume, basefre, po->output.buffer, po->output.size, po->sampfre, (uint32_t) (pos * po->sampfre));
		}
		End:
		return po;
	}
	Err:
	return NULL;
}

phoneme_output_s* phoneme_output_join(phoneme_output_s *restrict po)
{
	phoneme_output_s *r;
	uint32_t i, n;
	r = po;
	for (i = 0, n = po->core_number; i < n; ++i)
	{
		if (!phoneme_output_core_join(po->core + i, &po->output))
			r = NULL;
	}
	return r;
}
