#include "container_id.h"
#include <memory.h>

static void media_container_id_free_func(struct media_container_id_s *restrict r)
{
	if (r->name) refer_free(r->name);
	if (r->judge) refer_free(r->judge);
}

struct media_container_id_s* media_container_id_alloc(const char *restrict name, media_container_initial_judge_f initial, const struct media_container_id_func_t *restrict func)
{
	struct media_container_id_s *restrict r;
	struct media_attr_judge_s *restrict judge;
	if (name && func && (r = (struct media_container_id_s *) refer_alloz(sizeof(struct media_container_id_s))))
	{
		refer_set_free(r, (refer_free_f) media_container_id_free_func);
		if ((r->name = refer_dump_string(name)))
		{
			if (initial)
			{
				if (!(r->judge = judge = media_attr_judge_alloc()))
					goto label_fail;
				if (!initial(judge))
					goto label_fail;
			}
			memcpy(&r->func, func, sizeof(r->func));
			return r;
		}
		label_fail:
		refer_free(r);
	}
	return NULL;
}
