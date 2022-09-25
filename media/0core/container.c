#include "container.h"

static void media_container_free_func(struct media_container_s *restrict r)
{
	if (r->stream) refer_free(r->stream);
	if (r->attr) refer_free(r->attr);
	if (r->io) refer_free(r->io);
	if (r->pri_data) refer_free(r->pri_data);
	if (r->media) refer_free(r->media);
	if (r->id) refer_free(r->id);
}

struct media_container_s* media_container_alloc(const struct media_s *restrict media, const struct media_container_id_s *restrict container_id)
{
	struct media_container_s *restrict r;
	if ((r = (struct media_container_s *) refer_alloz(sizeof(struct media_container_s))))
	{
		refer_set_free(r, (refer_free_f) media_container_free_func);
		r->id = (const struct media_container_id_s *) refer_save(container_id);
		r->media = (const struct media_s *) refer_save(media);
		if (container_id->func.create && !(r->pri_data = container_id->func.create()))
			goto label_fail;
		if (!(r->attr = media_attr_alloc(NULL)))
			goto label_fail;
		if (container_id->judge)
			media_attr_set_judge(r->attr, container_id->judge, r->pri_data);
		if (!(r->stream = vattr_alloc()))
			goto label_fail;
		return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}
