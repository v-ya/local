#include "container.bmp.h"
#include "../0core/endian.h"

static struct media_container_inner_s* media_container_bmp_parse_info_12(struct media_container_inner_s *restrict ci)
{
	struct mi_bmp_info_12_t info;
	if (media_io_read(ci->io, &info, sizeof(info)) == sizeof(info))
	{
		// media_mlog_print_rawdata(c->media->mlog_verbose, "mi_bmp_info_12_t", &info, sizeof(info));
		if (media_attr_set_int(ci->attr, media_nai_width, media_n2le_16(info.width)) &&
			media_attr_set_int(ci->attr, media_nai_height, media_n2le_16(info.height)) &&
			media_attr_set_int(ci->attr, media_nai_bmp_color_plane, media_n2le_16(info.color_plane)) &&
			media_attr_set_int(ci->attr, media_nai_bpp, media_n2le_16(info.bpp))
			) return ci;
	}
	return NULL;
}

static struct media_container_inner_s* media_container_bmp_parse_info_40(struct media_container_inner_s *restrict ci, struct media_container_pri_bmp_s *restrict pri)
{
	struct mi_bmp_info_40_t info;
	if (media_io_read(ci->io, &info, sizeof(info)) == sizeof(info))
	{
		// media_mlog_print_rawdata(c->media->mlog_verbose, "mi_bmp_info_40_t", &info, sizeof(info));
		pri->image_size = media_n2le_32(info.image_size);
		if (media_attr_set_int(ci->attr, media_nai_width, media_n2le_32(info.width)) &&
			media_attr_set_int(ci->attr, media_nai_height, media_n2le_32(info.height)) &&
			media_attr_set_int(ci->attr, media_nai_bmp_color_plane, media_n2le_16(info.color_plane)) &&
			media_attr_set_int(ci->attr, media_nai_bpp, media_n2le_16(info.bpp)) &&
			media_attr_set_int(ci->attr, media_nai_bmp_compression, media_n2le_32(info.compression_method)) &&
			media_attr_set_int(ci->attr, media_nai_bmp_xppm, media_n2le_32(info.xppm)) &&
			media_attr_set_int(ci->attr, media_nai_bmp_yppm, media_n2le_32(info.yppm)) &&
			media_attr_set_int(ci->attr, media_nai_bmp_color_palette, media_n2le_32(info.color_palette)) &&
			media_attr_set_int(ci->attr, media_nai_bmp_used_color, media_n2le_32(info.used_color))
			) return ci;
	}
	return NULL;
}

d_media_container__parse_head(bmp)
{
	struct media_container_inner_s *restrict ci;
	const struct media_container_id_bmp_s *restrict id;
	const struct media_s *restrict m;
	struct media_io_s *restrict io;
	struct media_stream_s *restrict st;
	struct media_container_pri_bmp_s *restrict pri;
	struct mi_bmp_header_t bmp_header;
	struct media_stack__oz_t oz;
	refer_string_t magic;
	uint32_t offset, size, version;
	ci = c->inner;
	id = (const struct media_container_id_bmp_s *) ci->id;
	m = ci->media;
	io = ci->io;
	pri = (struct media_container_pri_bmp_s *) ci->pri_data;
	// get bmp header
	if (media_io_read(io, &bmp_header, sizeof(bmp_header)) != sizeof(bmp_header))
		goto label_fail;
	// media_mlog_print_rawdata(m->mlog_verbose, "bmp-header", &bmp_header, sizeof(bmp_header));
	if (!(magic = (refer_string_t) hashmap_get_head(&id->magic, (uint64_t) bmp_header.magic)))
		goto label_fail;
	if (!media_attr_refer_string(ci->attr, media_nas_bmp_magic, magic))
		goto label_fail;
	offset = sizeof(bmp_header);
	size = media_n2le_32(bmp_header.bmp_file_size);
	pri->pixel_offset = media_n2le_32(bmp_header.pixel_offset);
	// get bmp info size
	if ((offset + sizeof(version)) > size)
		goto label_fail;
	if (media_io_read(io, &version, sizeof(version)) != sizeof(version))
		goto label_fail;
	version = media_n2le_32(version);
	if (!media_attr_set_int(ci->attr, media_nai_bmp_version, version))
		goto label_fail;
	// get bmp info
	if ((offset += version) > size)
		goto label_fail;
	switch (version)
	{
		case 12:
			if (media_container_bmp_parse_info_12(ci)) break;
			goto label_fail;
		case 40:
			if (media_container_bmp_parse_info_40(ci, pri)) break;
			goto label_fail;
		default: goto label_fail;
	}
	// get bits-mask or color-table
	// config stream
	st = NULL;
	oz.offset = (uint64_t) pri->pixel_offset;
	oz.size = (uintptr_t) pri->image_size;
	if (pri->compression == 0)
	{
		if (!oz.size) oz.size = (((uintptr_t) pri->bpp * pri->width + 31) / 32 * 4) * pri->height;
		switch (pri->bpp)
		{
			case 24: st = media_container_new_stream(c, media_st_image, media_nf_bgr24_p4); break;
			case 32: st = media_container_new_stream(c, media_st_image, media_nf_bgra32); break;
			default: break;
		}
	}
	media_verbose(m, "compression: %u, bpp: %u, st: ... %p", pri->compression, pri->bpp, st);
	if (st && media_stack_push(st->stack, &oz))
		return c;
	label_fail:
	media_warning(m, "image/bmp parse head ... fail");
	return NULL;
}
