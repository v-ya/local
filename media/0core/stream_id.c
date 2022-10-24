#include "stream_id.h"
#include <memory.h>

void media_stream_id_free_func(struct media_stream_id_s *restrict r)
{
	if (r->name) refer_free(r->name);
	if (r->stream_type) refer_free(r->stream_type);
	if (r->frame_id_name) refer_free(r->frame_id_name);
	if (r->stack_layout) refer_free(r->stack_layout);
	if (r->judge) refer_free(r->judge);
}

struct media_stream_id_s* media_stream_id_alloc(uintptr_t size, const char *restrict name, refer_string_t stream_type, refer_string_t frame_id_name, refer_string_t stack_layout, const struct media_stream_id_func_t *restrict func)
{
	struct media_stream_id_s *restrict r;
	if (size >= sizeof(struct media_stream_id_s) && name && stream_type && frame_id_name && stack_layout && func &&
		(r = (struct media_stream_id_s *) refer_alloz(size)))
	{
		refer_set_free(r, (refer_free_f) media_stream_id_free_func);
		if ((r->name = refer_dump_string(name)))
		{
			r->stream_type = (refer_string_t) refer_save(stream_type);
			r->frame_id_name = (refer_string_t) refer_save(frame_id_name);
			r->stack_layout = (refer_string_t) refer_save(stack_layout);
			if (!func->initial_judge || (r->judge = media_attr_judge_create(func->initial_judge)))
			{
				memcpy(&r->func, func, sizeof(r->func));
				return r;
			}
		}
		refer_free(r);
	}
	return NULL;
}
