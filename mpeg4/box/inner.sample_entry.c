#include "inner.sample_entry.h"
#include "inner.data.h"
#include <string.h>

inner_visual_sample_t* mpeg4$define(inner, visual_sample, get)(inner_visual_sample_t *restrict r, const uint8_t *restrict *restrict data, uint64_t *restrict size)
{
	inner_visual_sample_entry_t t;
	if (*size >= sizeof(t))
	{
		size_t n;
		memcpy(&t, *data, sizeof(t));
		*data += sizeof(t);
		*size -= sizeof(t);
		r->data_reference_index = mpeg4_n16(t.sample_entry.data_reference_index);
		r->width = mpeg4_n16(t.width);
		r->height = mpeg4_n16(t.height);
		r->horizre_solution = mpeg4_fix_point(mpeg4_n32(t.horizre_solution), 16, 16);
		r->vertre_solution = mpeg4_fix_point(mpeg4_n32(t.vertre_solution), 16, 16);
		r->frame_count = mpeg4_n16(t.frame_count);
		r->depth = mpeg4_n16(t.depth);
		n = (size_t) t.compressor_name[0];
		if (n > 31) n = 31;
		if (n) memcpy(r->compressor_name, t.compressor_name + 1, n);
		r->compressor_name[n] = 0;
		return r;
	}
	return NULL;
}

uint8_t* mpeg4$define(inner, visual_sample, set)(uint8_t *restrict data, const inner_visual_sample_t *restrict r)
{
	inner_visual_sample_entry_t t;
	size_t n;
	memset(&t, 0, sizeof(t));
	t.sample_entry.data_reference_index = mpeg4_n16(r->data_reference_index);
	t.width = mpeg4_n16(r->width);
	t.height = mpeg4_n16(r->height);
	t.horizre_solution = mpeg4_n32(mpeg4_fix_point_gen(r->horizre_solution, uint32_t, 16, 16));
	t.vertre_solution = mpeg4_n32(mpeg4_fix_point_gen(r->vertre_solution, uint32_t, 16, 16));
	t.frame_count = mpeg4_n16(r->frame_count);
	if ((t.compressor_name[0] = (uint8_t) (n = strlen(r->compressor_name))))
		memcpy(t.compressor_name + 1, r->compressor_name, n);
	t.depth = mpeg4_n16(r->depth);
	t.pre_defined_1 = mpeg4_n16(-1);
	memcpy(data, &t, sizeof(t));
	return data + sizeof(t);
}

inner_audio_sample_t* mpeg4$define(inner, audio_sample, get)(inner_audio_sample_t *restrict r, const uint8_t *restrict *restrict data, uint64_t *restrict size)
{
	inner_audio_sample_entry_t t;
	if (*size >= sizeof(t))
	{
		memcpy(&t, *data, sizeof(t));
		*data += sizeof(t);
		*size -= sizeof(t);
		r->data_reference_index = mpeg4_n16(t.sample_entry.data_reference_index);
		r->entry_version = mpeg4_n16(t.entry_version);
		r->channel_count = mpeg4_n16(t.channel_count);
		r->sample_size = mpeg4_n16(t.sample_size);
		r->sample_rate = mpeg4_fix_point(mpeg4_n32(t.sample_rate), 16, 16);
		return r;
	}
	return NULL;
}

uint8_t* mpeg4$define(inner, audio_sample, set)(uint8_t *restrict data, const inner_audio_sample_t *restrict r)
{
	inner_audio_sample_entry_t t;
	memset(&t, 0, sizeof(t));
	t.sample_entry.data_reference_index = mpeg4_n16(r->data_reference_index);
	t.entry_version = mpeg4_n16(r->entry_version);
	t.channel_count = mpeg4_n16(r->channel_count);
	t.sample_size = mpeg4_n16(r->sample_size);
	t.sample_rate = mpeg4_n32(mpeg4_fix_point_gen(r->sample_rate, uint32_t, 16, 16));
	memcpy(data, &t, sizeof(t));
	return data + sizeof(t);
}
