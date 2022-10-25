#include "container_id.h"
#include <memory.h>

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
