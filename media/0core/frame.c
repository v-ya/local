#include "frame.h"

static void media_frame_free_func(struct media_frame_s *restrict r)
{
	uintptr_t i;
	for (i = 0; i < r->channel; ++i)
	{
		if (r->channel_chip[i])
			refer_free(r->channel_chip[i]);
	}
	if (r->data_source) refer_free(r->data_source);
	if (r->id) refer_free(r->id);
}

struct media_frame_s* media_frame_alloc(const struct media_frame_id_s *restrict frame_id, uintptr_t dimension, const uintptr_t dimension_value[])
{
	struct media_frame_s *restrict r;
	uintptr_t i, channel;
	if (frame_id->dimension == dimension && (r = (struct media_frame_s *) refer_alloz(
		sizeof(struct media_frame_s) + sizeof(struct media_channel_s *) * (channel = frame_id->channel))))
	{
		refer_set_free(r, (refer_free_f) media_frame_free_func);
		r->id = (const struct media_frame_id_s *) refer_save(frame_id);
		r->channel = channel;
		for (i = 0; i < channel; ++i)
		{
			if (!(r->channel_chip[i] = media_frame_id_create_channel(frame_id, i, dimension, dimension_value)))
				goto label_fail;
		}
		return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}
