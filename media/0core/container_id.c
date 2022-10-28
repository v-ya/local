#include "container_id.h"
#include "stream_spec.h"
#include <string.h>

void media_container_id_free_func(struct media_container_id_s *restrict r)
{
	if (r->name) refer_free(r->name);
	if (r->stream_spec) refer_free(r->stream_spec);
	if (r->judge) refer_free(r->judge);
}

struct media_container_id_s* media_container_id_alloc(uintptr_t size, const char *restrict name, const struct media_container_id_func_t *restrict func)
{
	struct media_container_id_s *restrict r;
	if (size >= sizeof(struct media_container_id_s) && name && func &&
		(r = (struct media_container_id_s *) refer_alloz(size)))
	{
		refer_set_free(r, (refer_free_f) media_container_id_free_func);
		if ((r->name = refer_dump_string(name)) && (r->stream_spec = vattr_alloc()))
		{
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

const struct media_stream_spec_s* media_container_id_get_spec(const struct media_container_id_s *restrict id, const char *restrict stream_type, const char *restrict frame_name)
{
	const struct media_stream_spec_s *restrict spec;
	const vattr_vlist_t *restrict vl;
	if (stream_type)
	{
		vl = vattr_get_vlist_first(id->stream_spec, stream_type);
		while (vl)
		{
			spec = (const struct media_stream_spec_s *) vl->value;
			if (!frame_name || !strcmp(spec->frame_id_name, frame_name))
				return spec;
			vl = vl->vslot_next;
		}
	}
	return NULL;
}
