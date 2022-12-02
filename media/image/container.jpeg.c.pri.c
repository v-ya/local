#include "container.jpeg.h"

static void media_container_pri_jpeg_free_func(struct media_container_pri_jpeg_s *restrict r)
{
	if (r->codec) refer_free(r->codec);
}

d_media_container__create_pri(jpeg)
{
	struct media_container_pri_jpeg_s *restrict r;
	if ((r = (struct media_container_pri_jpeg_s *) refer_alloz(sizeof(struct media_container_pri_jpeg_s))))
	{
		refer_set_free(r, (refer_free_f) media_container_pri_jpeg_free_func);
		if ((r->codec = mi_jpeg_codec_alloc()))
			return r;
		refer_free(r);
	}
	return NULL;
}
