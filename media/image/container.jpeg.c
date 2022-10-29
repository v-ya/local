#include "container.jpeg.h"

static void media_container_id_jpeg_free_func(struct media_container_id_jpeg_s *restrict r)
{
	media_container_id_free_func(&r->id);
}

struct media_container_id_s* media_container_create_image_jpeg(const struct media_s *restrict media)
{
	struct media_container_id_func_t func;
	struct media_container_id_jpeg_s *restrict r;
	d_media_container_func_initial(func, jpeg);
	if ((r = (struct media_container_id_jpeg_s *) media_container_id_alloc(sizeof(struct media_container_id_jpeg_s), media_nc_jpeg, &func)))
	{
		refer_set_free(r, (refer_free_f) media_container_id_jpeg_free_func);
		return &r->id;
		// refer_free(r);
	}
	return NULL;
}
