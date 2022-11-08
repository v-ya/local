#include "transform_id.h"
#include "media.h"
#include <memory.h>

void media_transform_id_free_func(struct media_transform_id_s *restrict r)
{
	if (r->src_frame_compat) refer_free(r->src_frame_compat);
	if (r->dst_frame_compat) refer_free(r->dst_frame_compat);
	if (r->judge) refer_free(r->judge);
}

struct media_transform_id_s* media_transform_id_alloc(uintptr_t size, const struct media_s *restrict media, const char *restrict sfid, const char *restrict dfid, const struct media_transform_id_func_t *restrict func)
{
	struct media_transform_id_s *restrict r;
	if (size >= sizeof(struct media_transform_id_s) && sfid && dfid && func &&
		(r = (struct media_transform_id_s *) refer_alloz(size)))
	{
		refer_set_free(r, (refer_free_f) media_transform_id_free_func);
		if ((r->src_frame_compat = media_save_string(media, sfid)) &&
			(!dfid || (r->dst_frame_compat = media_save_string(media, dfid))))
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
