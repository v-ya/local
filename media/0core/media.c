#include "media.h"

static void media_free_func(struct media_s *restrict r)
{
	hashmap_uini(&r->frame);
}

struct media_s* media_alloc_empty(void)
{
	struct media_s *restrict r;
	if ((r = (struct media_s *) refer_alloz(sizeof(struct media_s))))
	{
		refer_set_free(r, (refer_free_f) media_free_func);
		if (hashmap_init(&r->frame))
			return r;
		refer_free(r);
	}
	return NULL;
}

static void media_hashmap_free_refer_func(hashmap_vlist_t *restrict vl)
{
	if (vl->value) refer_free(vl->value);
}

static refer_t media_initial_hashmap_add_refer(hashmap_t *restrict hm, const char *restrict name, refer_t value)
{
	if (!hashmap_find_name(hm, name) && hashmap_set_name(hm, name, value, media_hashmap_free_refer_func))
		return refer_save(value);
	return NULL;
}

struct media_s* media_initial_add_frame(struct media_s *restrict media, const struct media_frame_id_s *restrict frame_id)
{
	if (frame_id && media_initial_hashmap_add_refer(&media->frame, frame_id->name, frame_id))
		return media;
	return NULL;
}