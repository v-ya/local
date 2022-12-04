#include "container.jpeg.h"

static void media_stream_pri_jpeg_free_func(struct media_stream_pri_jpeg_s *restrict r)
{
	if (r->codec) refer_free(r->codec);
}

d_media_stream__create_pri(jpeg)
{
	struct media_stream_pri_jpeg_s *restrict r;
	if ((r = (struct media_stream_pri_jpeg_s *) refer_alloz(sizeof(struct media_stream_pri_jpeg_s))))
	{
		refer_set_free(r, (refer_free_f) media_stream_pri_jpeg_free_func);
		if ((r->codec = (struct mi_jpeg_codec_s *) refer_save(c->inner->pri_data)))
			return r;
		refer_free(r);
	}
	return NULL;
}
