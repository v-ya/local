#include "container.bmp.h"

static struct media_container_inner_s* media_container_bmp_parse_header(struct media_container_inner_s *restrict ci)
{
	const struct media_container_id_bmp_s *restrict id;
	struct media_container_pri_bmp_s *restrict pri;
	struct mi_bmp_header_t bmp_header;
	refer_string_t magic;
	uint32_t version;
	id = (const struct media_container_id_bmp_s *) ci->id;
	pri = (struct media_container_pri_bmp_s *) ci->pri_data;
	if (media_io_read(ci->io, &bmp_header, sizeof(bmp_header)) != sizeof(bmp_header))
		goto label_fail;
	if (!(magic = (refer_string_t) hashmap_get_head(&id->magic, (uint64_t) bmp_header.magic)))
		goto label_fail;
	if (!media_attr_refer_string(ci->attr, media_nacs_bmp_magic, magic))
		goto label_fail;
	pri->pixel_offset = media_n2le_32(bmp_header.pixel_offset);
	if (media_io_read(ci->io, &version, sizeof(version)) != sizeof(version))
		goto label_fail;
	version = media_n2le_32(version);
	if (!media_attr_set_int(ci->attr, media_naci_bmp_version, version))
		goto label_fail;
	return ci;
	label_fail:
	return NULL;
}

static struct media_container_inner_s* media_container_bmp_parse_info_12(struct media_container_inner_s *restrict ci)
{
	struct mi_bmp_info_12_t info;
	if (media_io_read(ci->io, &info, sizeof(info)) == sizeof(info))
	{
		// media_mlog_print_rawdata(ci->media->mlog_verbose, "mi_bmp_info_12_t", &info, sizeof(info));
		if (media_attr_set_int(ci->attr, media_naci_width, media_n2le_16(info.width)) &&
			media_attr_set_int(ci->attr, media_naci_height, media_n2le_16(info.height)) &&
			media_attr_set_int(ci->attr, media_naci_bmp_color_plane, media_n2le_16(info.color_plane)) &&
			media_attr_set_int(ci->attr, media_naci_bpp, media_n2le_16(info.bpp))
			) return ci;
	}
	return NULL;
}

static struct media_container_inner_s* media_container_bmp_parse_info_40(struct media_container_inner_s *restrict ci, struct media_container_pri_bmp_s *restrict pri)
{
	struct mi_bmp_info_40_t info;
	int32_t height, y_reverse;
	if (media_io_read(ci->io, &info, sizeof(info)) == sizeof(info))
	{
		// media_mlog_print_rawdata(ci->media->mlog_verbose, "mi_bmp_info_40_t", &info, sizeof(info));
		pri->image_size = media_n2le_32(info.image_size);
		if ((height = media_n2le_32(info.height)) < 0)
		{
			height = -height;
			y_reverse = 1;
		}
		else y_reverse = 0;
		if (media_attr_set_int(ci->attr, media_naci_width, media_n2le_32(info.width)) &&
			media_attr_set_int(ci->attr, media_naci_height, height) &&
			media_attr_set_int(ci->attr, media_naci_bmp_color_plane, media_n2le_16(info.color_plane)) &&
			media_attr_set_int(ci->attr, media_naci_bpp, media_n2le_16(info.bpp)) &&
			media_attr_set_int(ci->attr, media_naci_bmp_compression, media_n2le_32(info.compression_method)) &&
			media_attr_set_int(ci->attr, media_naci_bmp_xppm, media_n2le_32(info.xppm)) &&
			media_attr_set_int(ci->attr, media_naci_bmp_yppm, media_n2le_32(info.yppm)) &&
			media_attr_set_int(ci->attr, media_naci_bmp_color_palette, media_n2le_32(info.color_palette)) &&
			media_attr_set_int(ci->attr, media_naci_bmp_used_color, media_n2le_32(info.used_color)) &&
			media_attr_set_int(ci->attr, media_naci_bmp_y_reverse, y_reverse)
			) return ci;
	}
	return NULL;
}

static struct media_container_inner_s* media_container_bmp_parse_info(struct media_container_inner_s *restrict ci)
{
	struct media_container_pri_bmp_s *restrict pri;
	pri = (struct media_container_pri_bmp_s *) ci->pri_data;
	switch (pri->version)
	{
		case 12: return media_container_bmp_parse_info_12(ci);
		case 40: return media_container_bmp_parse_info_40(ci, pri);
		default: return NULL;
	}
}

d_media_container__parse_head(bmp)
{
	struct media_container_inner_s *restrict ci;
	ci = c->inner;
	if (!media_container_bmp_parse_header(ci))
		goto label_fail;
	if (!media_container_bmp_parse_info(ci))
		goto label_fail;
	// get bits-mask or color-table
	return c;
	label_fail:
	media_warning(ci->media, "image/bmp parse head ... fail");
	return NULL;
}

d_media_container__parse_tail(bmp)
{
	struct media_container_inner_s *restrict ci;
	struct media_container_pri_bmp_s *restrict pri;
	struct media_stream_s *restrict st;
	struct media_stack__oz_t oz;
	ci = c->inner;
	pri = (struct media_container_pri_bmp_s *) ci->pri_data;
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
	if (st && media_stack_push(st->stack, &oz))
		return c;
	media_warning(ci->media, "image/bmp parse tail ... fail");
	return NULL;
}
