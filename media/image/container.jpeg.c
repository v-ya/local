#include "container.jpeg.h"

struct media_container_id_s* media_container_create_image_jpeg(const struct media_s *restrict media)
{
	struct media_container_id_s *restrict r;
	struct media_container_id_func_t func;
	d_media_container_func_initial(func, jpeg);
	if ((r = media_container_id_alloc(sizeof(struct media_container_id_s), media_nc_jpeg, &func)))
	{
		if (media_stream_symbol(spec_append, jpeg)(r->stream_spec, media))
			return r;
		refer_free(r);
	}
	return NULL;
}
