#include "phoneme_buffer.h"
#include <stdlib.h>
#include <memory.h>

static void phoneme_buffer_free_func(phoneme_buffer_s *restrict pb)
{
	if (pb->buffer) free(pb->buffer);
}

phoneme_buffer_s* phoneme_buffer_alloc(uint32_t sampfre, uint32_t frames)
{
	phoneme_buffer_s *r;
	r = refer_alloc(sizeof(phoneme_buffer_s));
	if (r)
	{
		r->sampfre = sampfre;
		r->frames_max = frames;
		r->frames = frames;
		if (!frames) frames = (16 << 10);
		r->frames_size = frames;
		r->buffer = (double *) calloc(frames, sizeof(double));
		if (r->buffer)
		{
			refer_set_free(r, (refer_free_f) phoneme_buffer_free_func);
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

phoneme_buffer_s* phoneme_buffer_set_sampfre(phoneme_buffer_s *restrict pb, uint32_t sampfre)
{
	if (!pb->frames_max)
	{
		pb->sampfre = sampfre;
		return pb;
	}
	return NULL;
}

phoneme_buffer_s* phoneme_buffer_set_frames(phoneme_buffer_s *restrict pb, double t)
{
	double *buffer;
	uint32_t frames, size;
	frames = (uint32_t) (t * pb->sampfre);
	if (frames <= pb->frames) goto label_return;
	if (frames <= pb->frames_size) goto label_set;
	if (!pb->frames_max)
	{
		size = pb->frames_size;
		while (size < 0x10000000u)
		{
			size <<= 1;
			if (frames <= size)
				goto label_resize;
		}
	}
	Err:
	return NULL;
	label_resize:
	buffer = realloc(pb->buffer, size * sizeof(double));
	if (!buffer) goto Err;
	memset((pb->buffer = buffer) + pb->frames_size, 0, (size - pb->frames_size) * sizeof(double));
	pb->frames_size = size;
	label_set:
	pb->frames = frames;
	label_return:
	return pb;
}

phoneme_buffer_s* phoneme_buffer_gen_note(phoneme_buffer_s *restrict pb, note_s *restrict note, double pos, double length, double volume, double basefre)
{
	if (length > 0 && volume > 0 && basefre > 0)
	{
		if (!pb->sampfre) return pb;
		else if (phoneme_buffer_set_frames(pb, pos + length))
		{
			note_gen_with_pos(note, length, volume, basefre, pb->buffer, pb->frames, pb->sampfre, (uint32_t) (pos * pb->sampfre));
			return pb;
		}
	}
	return NULL;
}
