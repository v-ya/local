#include "media.h"
#include "0core/attr.h"
#include "0core/frame.h"
#include "0core/media.h"
#include "media.frame.h"
// frame
#include "image/frame.h"
#include "zarch/frame.h"
// container
#include "image/container.h"
// print
#include <inttypes.h>

static media_s* media_alloc_add_frame(media_s *restrict r, struct media_frame_id_s* (*create_func)(void))
{
	struct media_frame_id_s *restrict id;
	if ((id = create_func()))
	{
		r = media_initial_add_frame(r, id);
		if (r) media_verbose(r, "add frame (%s) okay", id->name);
		else media_error(r, "add frame (%s) fail", id->name);
		refer_free(id);
		return r;
	}
	else media_error(r, "create frame id (%p) fail", create_func);
	return NULL;
}

static media_s* media_alloc_add_container(media_s *restrict r, struct media_container_id_s* (*create_func)(void))
{
	struct media_container_id_s *restrict id;
	if ((id = create_func()))
	{
		r = media_initial_add_container(r, id);
		if (r) media_verbose(r, "add container (%s) okay", id->name);
		else media_error(r, "add container (%s) fail", id->name);
		refer_free(id);
		return r;
	}
	else media_error(r, "create container id (%p) fail", create_func);
	return NULL;
}

const media_s* media_alloc(media_loglevel_t loglevel, struct mlog_s *restrict mlog)
{
	media_s *restrict r;
	if ((r = media_alloc_empty()))
	{
		if (loglevel && mlog)
			media_initial_set_mlog(r, mlog, (uint32_t) loglevel);
		// register ... audio image video zarch
		if (
			// frame
			media_alloc_add_frame(r, media_frame_create_image_bgra32) &&
			media_alloc_add_frame(r, media_frame_create_zarch_native) &&
			// container
			media_alloc_add_container(r, media_container_create_image_bmp)
		) return r;
		refer_free(r);
	}
	return NULL;
}

struct mlog_s* media_get_mlog(const media_s *restrict media, media_loglevel_t loglevel)
{
	return media_get_mlog_by_loglevel(media, (uint32_t) loglevel);
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

static void media_attr_dump_item(struct mlog_s *restrict mlog, const char *restrict name, const struct media_attr_item_s *restrict item)
{
	switch (item->type)
	{
		case media_attr_type__int:
			mlog_printf(mlog, "%s [int]: %" PRId64, name, item->value.av_int);
			break;
		case media_attr_type__float:
			mlog_printf(mlog, "%s [float]: %g", name, item->value.av_float);
			break;
		case media_attr_type__string:
			mlog_printf(mlog, "%s [string]: %s", name, item->value.av_string?item->value.av_string:"(null)");
			break;
		case media_attr_type__data:
			if (item->value.av_data)
				media_mlog_print_rawdata(mlog, name, item->value.av_data->string, item->value.av_data->length);
			else media_mlog_print_rawdata(mlog, name, NULL, 0);
			break;
		case media_attr_type__ptr:
			mlog_printf(mlog, "%s [ptr]: %p", name, item->value.av_ptr);
			break;
		default:
			mlog_printf(mlog, "%s [unknow]", name);
			break;
	}
}

void media_attr_dump(const media_attr_s *restrict attr, const media_s *restrict media, media_loglevel_t loglevel, const char *restrict name)
{
	struct mlog_s *restrict mlog;
	if (attr && (mlog = media_get_mlog(media, loglevel)))
	{
		const vattr_vlist_t *restrict vl;
		const vattr_vslot_t *restrict vslot;
		if (!name)
		{
			for (vl = attr->attr->vattr; vl; vl = vl->vattr_next)
				media_attr_dump_item(mlog, vl->vslot->key, (const struct media_attr_item_s *) vl->value);
		}
		else if ((vslot = vattr_get_vslot(attr->attr, name)))
		{
			for (vl = vslot->vslot; vl; vl = vl->vslot_next)
				media_attr_dump_item(mlog, name, (const struct media_attr_item_s *) vl->value);
		}
	}
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

// container

media_attr_s* media_container_get_attr(const media_container_s *restrict container)
{
	return container->attr;
}

media_container_s* media_create_container(const media_s *restrict media, const char *restrict frame_name)
{
	const struct media_container_id_s *restrict id;
	if (frame_name && (id = (const struct media_container_id_s *) hashmap_get_name(&media->container, frame_name)))
		return media_container_alloc(media, id);
	return NULL;
}

media_container_s* media_create_input_by_memory(const media_s *restrict media, const char *restrict frame_name, const void *data, uintptr_t size)
{
	media_container_s *restrict r, *rr;
	struct media_io_s *restrict io;
	if ((r = media_create_container(media, frame_name)))
	{
		if ((io = media_io_create_memory(data, size)))
		{
			rr = media_container_set_io(r, io, media_container_io_input);
			refer_free(io);
			if (rr) return r;
		}
		refer_free(r);
	}
	return NULL;
}
