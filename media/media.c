#include "media.h"
#include "0core/attr.h"
#include "0core/frame.h"
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

// attr

media_attr_s* media_attr_set_int(media_attr_s *restrict attr, const char *restrict name, int64_t value)
{
	if (attr && name)
		return media_attr_set(attr, name, media_attr_type__int, (union media_attr_value_t) {.av_int = value});
	return NULL;
}

media_attr_s* media_attr_set_float(media_attr_s *restrict attr, const char *restrict name, double value)
{
	if (attr && name)
		return media_attr_set(attr, name, media_attr_type__float, (union media_attr_value_t) {.av_float = value});
	return NULL;
}

media_attr_s* media_attr_set_string(media_attr_s *restrict attr, const char *restrict name, const char *restrict string)
{
	refer_string_t value;
	if (attr && name && string && (value = refer_dump_string(string)))
	{
		attr = media_attr_set(attr, name, media_attr_type__string, (union media_attr_value_t) {.av_string = value});
		refer_free(value);
		return attr;
	}
	return NULL;
}

media_attr_s* media_attr_set_data(media_attr_s *restrict attr, const char *restrict name, const void *data, uintptr_t size)
{
	refer_nstring_t value;
	if (attr && name && (value = refer_dump_nstring_with_length(data, size)))
	{
		attr = media_attr_set(attr, name, media_attr_type__data, (union media_attr_value_t) {.av_data = value});
		refer_free(value);
		return attr;
	}
	return NULL;
}

media_attr_s* media_attr_refer_string(media_attr_s *restrict attr, const char *restrict name, refer_string_t string)
{
	if (attr && name)
		return media_attr_set(attr, name, media_attr_type__string, (union media_attr_value_t) {.av_string = string});
	return NULL;
}

media_attr_s* media_attr_refer_data(media_attr_s *restrict attr, const char *restrict name, refer_nstring_t data)
{
	if (attr && name)
		return media_attr_set(attr, name, media_attr_type__data, (union media_attr_value_t) {.av_data = data});
	return NULL;
}

const media_attr_s* media_attr_get_int(const media_attr_s *restrict attr, const char *restrict name, int64_t *restrict value)
{
	union media_attr_value_t v;
	if (attr && name && media_attr_get(attr, name, media_attr_type__int, &v))
	{
		if (value) *value = v.av_int;
		return attr;
	}
	return NULL;
}

const media_attr_s* media_attr_get_float(const media_attr_s *restrict attr, const char *restrict name, double *restrict value)
{
	union media_attr_value_t v;
	if (attr && name && media_attr_get(attr, name, media_attr_type__float, &v))
	{
		if (value) *value = v.av_float;
		return attr;
	}
	return NULL;
}

const media_attr_s* media_attr_get_string(const media_attr_s *restrict attr, const char *restrict name, refer_string_t *restrict string)
{
	union media_attr_value_t v;
	if (attr && name && media_attr_get(attr, name, media_attr_type__string, &v))
	{
		if (string) *string = v.av_string;
		return attr;
	}
	return NULL;
}

const media_attr_s* media_attr_get_data(const media_attr_s *restrict attr, const char *restrict name, refer_nstring_t *restrict data)
{
	union media_attr_value_t v;
	if (attr && name && media_attr_get(attr, name, media_attr_type__data, &v))
	{
		if (data) *data = v.av_data;
		return attr;
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
