#include "media.h"
#include "0core/attr.h"
#include "0core/frame.h"
#include "0core/media.h"
#include "0core/runtime.h"
#include "0core/transform.h"
#include "media.frame.h"
#include "media.container.h"
// frame
#include "image/frame.h"
#include "zarch/frame.h"
// container
#include "image/container.h"
// transform
#include "image/transform.h"
// print
#include <inttypes.h>

static media_s* media_alloc_add_stack(media_s *restrict r, media_stack_layout_initial_f initial_func)
{
	struct media_stack_id_s *restrict id;
	media_s *restrict m;
	if ((id = media_stack_id_alloc(initial_func)))
	{
		r = media_initial_add_stack(m = r, id);
		if (r) media_verbose(m, "add stack (%s) okay", id->name);
		else media_error(m, "add stack (%s) fail", id->name);
		refer_free(id);
		return r;
	}
	else media_error(r, "create stack id (%p) fail", initial_func);
	return NULL;
}

static media_s* media_alloc_add_frame(media_s *restrict r, struct media_frame_id_s* (*create_func)(void))
{
	struct media_frame_id_s *restrict id;
	media_s *restrict m;
	if ((id = create_func()))
	{
		r = media_initial_add_frame(m = r, id);
		if (r) media_verbose(m, "add frame (%s) okay", id->name);
		else media_error(m, "add frame (%s) fail", id->name);
		refer_free(id);
		return r;
	}
	else media_error(r, "create frame id (%p) fail", create_func);
	return NULL;
}

static media_s* media_alloc_add_container(media_s *restrict r, struct media_container_id_s* (*create_func)(const struct media_s *restrict media))
{
	struct media_container_id_s *restrict id;
	media_s *restrict m;
	if ((id = create_func(r)))
	{
		r = media_initial_add_container(m = r, id);
		if (r) media_verbose(m, "add container (%s) okay", id->name);
		else media_error(m, "add container (%s) fail", id->name);
		refer_free(id);
		return r;
	}
	else media_error(r, "create container id (%p) fail", create_func);
	return NULL;
}

static media_s* media_alloc_add_transform(media_s *restrict r, struct media_transform_id_s* (*create_func)(const struct media_s *restrict media))
{
	struct media_transform_id_s *restrict id;
	media_s *restrict m;
	const char *restrict sc, *restrict dc;
	if ((id = create_func(r)))
	{
		sc = id->src_frame_compat;
		if (!(dc = id->dst_frame_compat))
			dc = "any";
		r = media_initial_add_transform(m = r, id);
		if (r) media_verbose(m, "add transform (%s => %s) okay", sc, dc);
		else media_error(m, "add transform (%s => %s) fail", sc, dc);
		refer_free(id);
		return r;
	}
	else media_error(r, "create transform id (%p) fail", create_func);
	return NULL;
}

const media_s* media_alloc(media_loglevel_t loglevel, struct mlog_s *restrict mlog)
{
	media_s *restrict r;
	if ((r = media_alloc_empty()))
	{
		if (loglevel && mlog)
			media_initial_set_mlog(r, mlog, (uint32_t) loglevel);
		// register ...
		if (
			// stream type
			media_initial_add_string(r, media_st_audio) &&
			media_initial_add_string(r, media_st_image) &&
			media_initial_add_string(r, media_st_video) &&
			media_initial_add_string(r, media_st_zarch) &&
			// stack layout
			media_alloc_add_stack(r, media_stack_layout_initial__oz) &&
			// frame
			media_alloc_add_frame(r, media_frame_create_image_bgra32) &&
			media_alloc_add_frame(r, media_frame_create_image_bgr24) &&
			media_alloc_add_frame(r, media_frame_create_image_bgr24_p4) &&
			media_alloc_add_frame(r, media_frame_create_zarch_native) &&
			// container
			media_alloc_add_container(r, media_container_create_image_bmp) &&
			media_alloc_add_container(r, media_container_create_image_jpeg) &&
			// transform
			media_alloc_add_transform(r, media_transform_create_image__bgr24_bgra32) &&
		1) return r;
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
	if (attr && name && string)
		return media_attr_set(attr, name, media_attr_type__string, (union media_attr_value_t) {.av_string = string});
	return NULL;
}

media_attr_s* media_attr_refer_data(media_attr_s *restrict attr, const char *restrict name, refer_nstring_t data)
{
	if (attr && name && data)
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
			mlog_printf(mlog, "%s [string]: %s", name, item->value.av_string);
			break;
		case media_attr_type__data:
			media_mlog_print_rawdata(mlog, name, item->value.av_data->string, item->value.av_data->length);
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

media_frame_s* media_create_frame(const media_s *restrict media, const char *restrict frame_name, uintptr_t d, const uintptr_t *restrict dv)
{
	const struct media_frame_id_s *restrict id;
	if (frame_name && (id = media_get_frame(media, frame_name)))
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

refer_string_t media_frame_get_name(const media_frame_s *restrict frame)
{
	return frame->id->name;
}

refer_string_t media_frame_get_compat(const media_frame_s *restrict frame)
{
	return frame->id->compat;
}

// container

media_attr_s* media_container_get_attr(const media_container_s *restrict container)
{
	return container->inner->attr;
}

media_io_s* media_container_get_io(const media_container_s *restrict container)
{
	return container->inner->io;
}

media_container_s* media_container_write_head(media_container_s *restrict container)
{
	return media_container_done_step(container, media_container_step_head);
}

media_container_s* media_container_write_tail(media_container_s *restrict container)
{
	return media_container_done_step(container, media_container_step_tail);
}

media_container_s* media_container_set_input(media_container_s *restrict container, media_io_s *restrict io)
{
	if (media_container_set_io(container, io, media_container_io_input))
		return media_container_done_step(container, media_container_step_tail);
	return NULL;
}

media_container_s* media_container_set_output(media_container_s *restrict container, media_io_s *restrict io)
{
	return media_container_set_io(container, io, media_container_io_output);
}

media_container_s* media_create_container(const media_s *restrict media, const char *restrict container_name)
{
	const struct media_container_id_s *restrict id;
	if (container_name && (id = media_get_container(media, container_name)))
		return media_container_alloc(media, id);
	return NULL;
}

media_container_s* media_create_input(const media_s *restrict media, const char *restrict container_name, media_io_s *restrict io)
{
	media_container_s *restrict r;
	if ((r = media_create_container(media, container_name)))
	{
		if (media_container_set_input(r, io))
			return r;
		refer_free(r);
	}
	return NULL;
}

media_container_s* media_create_output(const media_s *restrict media, const char *restrict container_name, media_io_s *restrict io)
{
	media_container_s *restrict r;
	if ((r = media_create_container(media, container_name)))
	{
		if (media_container_set_output(r, io))
			return r;
		refer_free(r);
	}
	return NULL;
}

media_container_s* media_create_input_by_memory(const media_s *restrict media, const char *restrict container_name, const void *data, uintptr_t size)
{
	media_container_s *restrict r;
	media_io_s *restrict io;
	r = NULL;
	if ((io = media_io_create_memory(data, size)))
	{
		r = media_create_input(media, container_name, io);
		refer_free(io);
	}
	return r;
}

media_container_s* media_create_output_by_memory(const media_s *restrict media, const char *restrict container_name)
{
	media_container_s *restrict r;
	media_io_s *restrict io;
	r = NULL;
	if ((io = media_io_create_memory(NULL, 0)))
	{
		r = media_create_output(media, container_name, io);
		refer_free(io);
	}
	return r;
}

media_container_s* media_create_input_by_path(const media_s *restrict media, const char *restrict container_name, const char *restrict path)
{
	media_container_s *restrict r;
	media_io_s *restrict io;
	r = NULL;
	if ((io = media_io_create_fsys(path, fsys_file_flag_read, 0, 0)))
	{
		r = media_create_input(media, container_name, io);
		refer_free(io);
	}
	return r;
}

media_container_s* media_create_output_by_path(const media_s *restrict media, const char *restrict container_name, const char *restrict path)
{
	media_container_s *restrict r;
	media_io_s *restrict io;
	r = NULL;
	if ((io = media_io_create_fsys(path, fsys_file_flag_read | fsys_file_flag_write | fsys_file_flag_create | fsys_file_flag_truncate, 0, 0)))
	{
		r = media_create_output(media, container_name, io);
		refer_free(io);
	}
	return r;
}

// stream

refer_string_t media_stream_get_frame_name(const media_stream_s *restrict stream)
{
	return stream->frame_id->name;
}
refer_string_t media_stream_get_frame_compat(const media_stream_s *restrict stream)
{
	return stream->frame_id->compat;
}

// io

uint64_t media_io_get_size(media_io_s *restrict io)
{
	return media_io_size(io);
}
uint64_t media_io_get_offset(media_io_s *restrict io)
{
	return media_io_offset(io, NULL);
}
uint64_t media_io_set_offset(media_io_s *restrict io, uint64_t offset)
{
	return media_io_offset(io, &offset);
}
uintptr_t media_io_read_data(media_io_s *restrict io, void *data, uintptr_t size)
{
	return media_io_read(io, data, size);
}
uintptr_t media_io_write_data(media_io_s *restrict io, const void *data, uintptr_t size)
{
	return media_io_write(io, data, size);
}
void* media_io_map_data(media_io_s *restrict io, uintptr_t *restrict rsize)
{
	return media_io_map(io, rsize);
}
media_io_s* media_io_sync_data(media_io_s *restrict io)
{
	return media_io_sync(io);
}

// transform

struct media_transform_s* media_create_transform(const media_s *restrict media, media_runtime_s *restrict runtime, const char *restrict src_frame_compat, const char *restrict dst_frame_compat)
{
	const struct media_transform_id_s *restrict id;
	if ((id = media_get_transform(media, src_frame_compat, dst_frame_compat)))
		return media_transform_alloc(media, id, runtime);
	return NULL;
}
