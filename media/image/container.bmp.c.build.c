#include "container.bmp.h"

static struct media_container_inner_s* media_container_bmp_build_header(struct media_container_inner_s *restrict ci, uint32_t bmp_file_size, uint32_t pixel_offset)
{
	struct media_container_pri_bmp_s *restrict pri;
	struct mi_bmp_header_t bmp_header;
	uint32_t version;
	pri = (struct media_container_pri_bmp_s *) ci->pri_data;
	bmp_header.magic = (uint16_t) pri->magic;
	bmp_header.bmp_file_size = media_n2le_32(bmp_file_size);
	bmp_header.reserve1 = 0;
	bmp_header.reserve2 = 0;
	bmp_header.pixel_offset = media_n2le_32(pixel_offset);
	version = media_n2le_32(pri->version);
	if (media_io_write(ci->io, &bmp_header, sizeof(bmp_header)) != sizeof(bmp_header))
		goto label_fail;
	if (media_io_write(ci->io, &version, sizeof(version)) != sizeof(version))
		goto label_fail;
	return ci;
	label_fail:
	return NULL;
}

static struct media_container_inner_s* media_container_bmp_build_info_12(struct media_container_inner_s *restrict ci)
{
	struct mi_bmp_info_12_t info;
	struct media_container_pri_bmp_s *restrict pri;
	pri = (struct media_container_pri_bmp_s *) ci->pri_data;
	info.width = media_n2le_16((uint16_t) pri->width);
	info.height = media_n2le_16((uint16_t) pri->height);
	info.color_plane = media_n2le_16((uint16_t) pri->color_plane);
	info.bpp = media_n2le_16((uint16_t) pri->bpp);
	if (media_io_write(ci->io, &info, sizeof(info)) == sizeof(info))
		return ci;
	return NULL;
}

static struct media_container_inner_s* media_container_bmp_build_info_40(struct media_container_inner_s *restrict ci, uint32_t image_size)
{
	struct mi_bmp_info_40_t info;
	struct media_container_pri_bmp_s *restrict pri;
	pri = (struct media_container_pri_bmp_s *) ci->pri_data;
	info.width = media_n2le_32((int32_t) pri->width);
	info.height = media_n2le_32((pri->y_reverse?-(int32_t) pri->height:(int32_t) pri->height));
	info.color_plane = media_n2le_16((uint16_t) pri->color_plane);
	info.bpp = media_n2le_16((uint16_t) pri->bpp);
	info.compression_method = media_n2le_32(pri->compression);
	info.image_size = media_n2le_32(image_size);
	info.xppm = media_n2le_32(pri->xppm);
	info.yppm = media_n2le_32(pri->yppm);
	info.color_palette = media_n2le_32(pri->color_palette);
	info.used_color = media_n2le_32(pri->used_color);
	if (media_io_write(ci->io, &info, sizeof(info)) == sizeof(info))
		return ci;
	return NULL;
}

static struct media_container_inner_s* media_container_bmp_build_info(struct media_container_inner_s *restrict ci, uint32_t image_size)
{
	struct media_container_pri_bmp_s *restrict pri;
	pri = (struct media_container_pri_bmp_s *) ci->pri_data;
	switch (pri->version)
	{
		case 12: return media_container_bmp_build_info_12(ci);
		case 40: return media_container_bmp_build_info_40(ci, image_size);
		default: return NULL;
	}
}

d_media_container__build_head(bmp)
{
	struct media_container_inner_s *restrict ci;
	ci = c->inner;
	if (!media_container_bmp_build_header(ci, 0, 0))
		goto label_fail;
	if (!media_container_bmp_build_info(ci, 0))
		goto label_fail;
	return c;
	label_fail:
	media_warning(ci->media, "image/bmp build head ... fail");
	return NULL;
}

d_media_container__build_tail(bmp)
{
	struct media_container_inner_s *restrict ci;
	struct media_container_pri_bmp_s *restrict pri;
	uint64_t offset;
	uint32_t bmp_file_size, pixel_offset, image_size;
	ci = c->inner;
	pri = (struct media_container_pri_bmp_s *) ci->pri_data;
	offset = 0;
	bmp_file_size = (uint32_t) media_io_size(ci->io);
	pixel_offset = pri->pixel_offset;
	image_size = pri->image_size;
	if (media_io_offset(ci->io, &offset) != 0)
		goto label_fail;
	if (!media_container_bmp_build_header(ci, bmp_file_size, pixel_offset))
		goto label_fail;
	if (!media_container_bmp_build_info(ci, image_size))
		goto label_fail;
	return c;
	label_fail:
	media_warning(ci->media, "image/bmp build tail ... fail");
	return NULL;
}
