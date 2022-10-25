#include "container.bmp.h"

static void media_container_pri_bmp_free_func(struct media_container_pri_bmp_s *restrict r)
{
	if (r->id) refer_free(r->id);
}

d_media_container__create_pri(bmp)
{
	struct media_container_pri_bmp_s *restrict r;
	if ((r = (struct media_container_pri_bmp_s *) refer_alloz(sizeof(struct media_container_pri_bmp_s))))
	{
		refer_set_free(r, (refer_free_f) media_container_pri_bmp_free_func);
		r->id = (const struct media_container_id_bmp_s *) refer_save(id);
	}
	return r;
}
