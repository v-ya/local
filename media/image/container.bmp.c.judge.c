#include "container.bmp.h"

static d_media_attr_unset(bmp, , struct media_container_pri_bmp_s)
{
	pri->pixel_offset = 0;
	pri->image_size = 0;
}
static d_media_attr_set(bmp, magic, struct media_container_pri_bmp_s)
{
	const char *restrict s;
	uint16_t magic;
	s = value->value.av_string;
	if (!s || !s[0] || !s[1] || s[2])
		goto label_fail;
	magic = *(uint16_t *) s;
	if (!hashmap_get_head(&pri->id->magic, (uint64_t) magic))
		goto label_fail;
	pri->magic = magic;
	return attr;
	label_fail:
	return NULL;
}
static d_media_attr_unset(bmp, magic, struct media_container_pri_bmp_s)
{
	pri->magic = *(uint16_t *) "BM";
}
static d_media_attr_set(bmp, version, struct media_container_pri_bmp_s)
{
	uint32_t version;
	switch ((version = (uint32_t) value->value.av_int))
	{
		case 12:
		case 16:
		case 40:
		case 52:
		case 56:
		case 64:
		case 108:
		case 124:
			pri->version = version;
			return attr;
		default: return NULL;
	}
}
static d_media_attr_unset(bmp, version, struct media_container_pri_bmp_s)
{
	pri->version = 40;
}
static d_media_attr_set(bmp, width, struct media_container_pri_bmp_s)
{
	pri->width = (int32_t) value->value.av_int;
	return attr;
}
static d_media_attr_unset(bmp, width, struct media_container_pri_bmp_s)
{
	pri->width = 0;
}
static d_media_attr_set(bmp, height, struct media_container_pri_bmp_s)
{
	pri->height = (int32_t) value->value.av_int;
	return attr;
}
static d_media_attr_unset(bmp, height, struct media_container_pri_bmp_s)
{
	pri->height = 0;
}
static d_media_attr_set(bmp, color_plane, struct media_container_pri_bmp_s)
{
	if (value->value.av_int != 1)
		return NULL;
	pri->color_plane = 1;
	return attr;
}
static d_media_attr_unset(bmp, color_plane, struct media_container_pri_bmp_s)
{
	pri->color_plane = 1;
}
static d_media_attr_set(bmp, bpp, struct media_container_pri_bmp_s)
{
	uint32_t bpp;
	switch ((bpp = (uint32_t) value->value.av_int))
	{
		// case 1:
		// case 4:
		// case 8:
		// case 16:
		case 24:
		case 32:
			pri->bpp = bpp;
			return attr;
		default: return NULL;
	}
}
static d_media_attr_unset(bmp, bpp, struct media_container_pri_bmp_s)
{
	pri->bpp = 32;
}
static d_media_attr_set(bmp, compression, struct media_container_pri_bmp_s)
{
	uint32_t compression;
	switch ((compression = (uint32_t) value->value.av_int))
	{
		case 0: // RGB
			pri->compression = compression;
			return attr;
		default: return NULL;
	}
}
static d_media_attr_unset(bmp, compression, struct media_container_pri_bmp_s)
{
	pri->compression = 0;
}
static d_media_attr_set(bmp, xppm, struct media_container_pri_bmp_s)
{
	pri->xppm = (int32_t) value->value.av_int;
	return attr;
}
static d_media_attr_unset(bmp, xppm, struct media_container_pri_bmp_s)
{
	pri->xppm = 0;
}
static d_media_attr_set(bmp, yppm, struct media_container_pri_bmp_s)
{
	pri->yppm = (int32_t) value->value.av_int;
	return attr;
}
static d_media_attr_unset(bmp, yppm, struct media_container_pri_bmp_s)
{
	pri->yppm = 0;
}
static d_media_attr_set(bmp, color_palette, struct media_container_pri_bmp_s)
{
	pri->color_palette = (int32_t) value->value.av_int;
	return attr;
}
static d_media_attr_unset(bmp, color_palette, struct media_container_pri_bmp_s)
{
	pri->color_palette = 0;
}
static d_media_attr_set(bmp, used_color, struct media_container_pri_bmp_s)
{
	pri->used_color = (int32_t) value->value.av_int;
	return attr;
}
static d_media_attr_unset(bmp, used_color, struct media_container_pri_bmp_s)
{
	pri->used_color = 0;
}

d_media_container__initial_judge(bmp)
{
	media_attr_judge_set_extra_clear(judge, (media_attr_unset_f) media_attr_symbol(unset, bmp, ));
	if (d_media_attr_judge_add(judge, bmp, magic, media_nas_bmp_magic, string) &&
		d_media_attr_judge_add(judge, bmp, version, media_nai_bmp_version, uint32) &&
		d_media_attr_judge_add(judge, bmp, width, media_nai_width, sint32) &&
		d_media_attr_judge_add(judge, bmp, height, media_nai_height, sint32) &&
		d_media_attr_judge_add(judge, bmp, color_plane, media_nai_bmp_color_plane, uint32) &&
		d_media_attr_judge_add(judge, bmp, bpp, media_nai_bpp, uint32) &&
		d_media_attr_judge_add(judge, bmp, compression, media_nai_bmp_compression, uint32) &&
		d_media_attr_judge_add(judge, bmp, xppm, media_nai_bmp_xppm, uint32) &&
		d_media_attr_judge_add(judge, bmp, yppm, media_nai_bmp_yppm, uint32) &&
		d_media_attr_judge_add(judge, bmp, color_palette, media_nai_bmp_color_palette, uint32) &&
		d_media_attr_judge_add(judge, bmp, used_color, media_nai_bmp_used_color, uint32)
	) return judge;
	return NULL;
}
