#include "stream.h"

#define media_stream__initial_judge__image__oz_bgra32 NULL

struct media_stream_id_s* media_stream_create_image_oz_bgra32(const struct media_s *restrict media)
{
	struct media_stream_id_func_t func;
	d_media_stream_func_initial(func, image, oz_bgra32);
	return media_stream_id_alloc(
		sizeof(struct media_stream_id_s), media_ns_oz_bgra32,
		media_get_string(media, media_st_image),
		media_get_string(media, media_nf_bgra32),
		media_get_string(media, media_sl_oz), &func);
}
