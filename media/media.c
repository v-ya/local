#include "media.h"
#include "0core/media.h"
#include "media.frame.h"
// frame
#include "image/frame.h"
#include "zarch/frame.h"

static inline media_s* media_alloc_add_frame(media_s *restrict r, struct media_frame_id_s* (*create_func)(void))
{
	struct media_frame_id_s *restrict id;
	if ((id = create_func()))
	{
		r = media_initial_add_frame(r, id);
		refer_free(id);
		return r;
	}
	return NULL;
}

const media_s* media_alloc(void)
{
	media_s *restrict r;
	if ((r = media_alloc_empty()))
	{
		// register ...
		if (
			// audio
			// image
			media_alloc_add_frame(r, media_frame_create_image_bgra32) &&
			// video
			// zarch
			media_alloc_add_frame(r, media_frame_create_zarch_native)
		) return r;
		refer_free(r);
	}
	return NULL;
}

// frame

media_frame_s* media_create_frame(const media_s *restrict media, const char *restrict frame_name, uintptr_t d, const uintptr_t dv[])
{
	const struct media_frame_id_s *restrict id;
	if (frame_name && (id = (const struct media_frame_id_s *) hashmap_get_name(&media->frame, frame_name)))
		return media_frame_alloc(id, d, dv);
	return NULL;
}

media_frame_s* media_create_frame_1d(const media_s *restrict media, const char *restrict frame_name, uintptr_t n)
{
	uintptr_t dv[1] = {n};
	return media_create_frame(media, frame_name, 1, dv);
}

media_frame_s* media_create_frame_2d(const media_s *restrict media, const char *restrict frame_name, uintptr_t width, uintptr_t height)
{
	uintptr_t dv[2] = {width, height};
	return media_create_frame(media, frame_name, 2, dv);
}

media_frame_s* media_create_frame_3d(const media_s *restrict media, const char *restrict frame_name, uintptr_t width, uintptr_t height, uintptr_t depth)
{
	uintptr_t dv[3] = {width, height, depth};
	return media_create_frame(media, frame_name, 3, dv);
}
