#include "container.h"
#include "../0core/endian.h"
#include "../media.attr.h"

struct media_container_id_bmp_s {
	struct media_container_id_s id;
	hashmap_t magic;
};

struct media_container_pri_bmp_s {
	const struct media_container_id_bmp_s *restrict id;
	uint32_t magic;
	uint32_t version;
	uint32_t pixel_offset;
	uint32_t image_size;
	// info ...
	int32_t width;
	int32_t height;
	uint16_t color_plane;
	uint16_t bpp;
	uint32_t compression;
	uint32_t xppm;
	uint32_t yppm;
	uint32_t color_palette;
	uint32_t used_color;
};

// packed
//     file-header    (mi_bmp_header_t)  must
//     info-size      (uint32_t)         must
//     info-data      (mi_bmp_info_*_t)  must
//     bit-mask       (uint32_t[])       maybe  (compression_method == BITFIELDS | ALPHABITFIELDS) => ([3] | [4])
//     color-table    *                  maybe  color depth <= 8
// insert
//     pixel-array    *                  must   pixels data, must linesize padding to 4 * n
//     color-profile  *                  maybe  ICC color profile

struct mi_bmp_header_t {
	uint16_t magic;
	uint32_t bmp_file_size;
	uint16_t reserve1;
	uint16_t reserve2;
	uint32_t pixel_offset;
} __attribute__ ((packed));

struct mi_bmp_info_12_t {
	uint16_t width;
	uint16_t height;
	uint16_t color_plane;
	uint16_t bpp;
} __attribute__ ((packed));

struct mi_bmp_info_40_t {
	int32_t width;
	int32_t height;
	uint16_t color_plane;
	uint16_t bpp;                 // 1, 4, 8, 16, 24, 32
	uint32_t compression_method;  // 0(RGB), 1(RLE8), 2(RLE4), 3(BITFIELDS), 4(JPEG), 5(PNG), 6(ALPHABITFIELDS), 11(CMYK), 12(CMYKRLE8), 13(CMYKRLE4)
	uint32_t image_size;          // size of the raw bitmap data (if RGB, image_size = 0)
	uint32_t xppm;
	uint32_t yppm;
	uint32_t color_palette;       // 0 and 2^n
	uint32_t used_color;          // 0 is every color
} __attribute__ ((packed));

// judge

static d_media_attr_unset(image, bmp, , struct media_container_pri_bmp_s)
{
	pri->pixel_offset = 0;
	pri->image_size = 0;
}
static d_media_attr_set(image, bmp, magic, struct media_container_pri_bmp_s)
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
static d_media_attr_unset(image, bmp, magic, struct media_container_pri_bmp_s)
{
	pri->magic = *(uint16_t *) "BM";
}
static d_media_attr_set(image, bmp, version, struct media_container_pri_bmp_s)
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
static d_media_attr_unset(image, bmp, version, struct media_container_pri_bmp_s)
{
	pri->version = 40;
}
static d_media_attr_set(image, bmp, width, struct media_container_pri_bmp_s)
{
	pri->width = (int32_t) value->value.av_int;
	return attr;
}
static d_media_attr_unset(image, bmp, width, struct media_container_pri_bmp_s)
{
	pri->width = 0;
}
static d_media_attr_set(image, bmp, height, struct media_container_pri_bmp_s)
{
	pri->height = (int32_t) value->value.av_int;
	return attr;
}
static d_media_attr_unset(image, bmp, height, struct media_container_pri_bmp_s)
{
	pri->height = 0;
}
static d_media_attr_set(image, bmp, color_plane, struct media_container_pri_bmp_s)
{
	if (value->value.av_int != 1)
		return NULL;
	pri->color_plane = 1;
	return attr;
}
static d_media_attr_unset(image, bmp, color_plane, struct media_container_pri_bmp_s)
{
	pri->color_plane = 1;
}
static d_media_attr_set(image, bmp, bpp, struct media_container_pri_bmp_s)
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
static d_media_attr_unset(image, bmp, bpp, struct media_container_pri_bmp_s)
{
	pri->bpp = 32;
}
static d_media_attr_set(image, bmp, compression, struct media_container_pri_bmp_s)
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
static d_media_attr_unset(image, bmp, compression, struct media_container_pri_bmp_s)
{
	pri->compression = 0;
}
static d_media_attr_set(image, bmp, xppm, struct media_container_pri_bmp_s)
{
	pri->xppm = (int32_t) value->value.av_int;
	return attr;
}
static d_media_attr_unset(image, bmp, xppm, struct media_container_pri_bmp_s)
{
	pri->xppm = 0;
}
static d_media_attr_set(image, bmp, yppm, struct media_container_pri_bmp_s)
{
	pri->yppm = (int32_t) value->value.av_int;
	return attr;
}
static d_media_attr_unset(image, bmp, yppm, struct media_container_pri_bmp_s)
{
	pri->yppm = 0;
}
static d_media_attr_set(image, bmp, color_palette, struct media_container_pri_bmp_s)
{
	pri->color_palette = (int32_t) value->value.av_int;
	return attr;
}
static d_media_attr_unset(image, bmp, color_palette, struct media_container_pri_bmp_s)
{
	pri->color_palette = 0;
}
static d_media_attr_set(image, bmp, used_color, struct media_container_pri_bmp_s)
{
	pri->used_color = (int32_t) value->value.av_int;
	return attr;
}
static d_media_attr_unset(image, bmp, used_color, struct media_container_pri_bmp_s)
{
	pri->used_color = 0;
}

static d_media_container__initial_judge(image, bmp)
{
	media_attr_judge_set_extra_clear(judge, (media_attr_unset_f) media_attr_symbol(unset, image, bmp, ));
	if (d_media_attr_judge_add(judge, image, bmp, magic, media_nas_bmp_magic, exist_string) &&
		d_media_attr_judge_add(judge, image, bmp, version, media_nai_bmp_version, uint32) &&
		d_media_attr_judge_add(judge, image, bmp, width, media_nai_width, sint32) &&
		d_media_attr_judge_add(judge, image, bmp, height, media_nai_height, sint32) &&
		d_media_attr_judge_add(judge, image, bmp, color_plane, media_nai_bmp_color_plane, uint32) &&
		d_media_attr_judge_add(judge, image, bmp, bpp, media_nai_bpp, uint32) &&
		d_media_attr_judge_add(judge, image, bmp, compression, media_nai_bmp_compression, uint32) &&
		d_media_attr_judge_add(judge, image, bmp, xppm, media_nai_bmp_xppm, uint32) &&
		d_media_attr_judge_add(judge, image, bmp, yppm, media_nai_bmp_yppm, uint32) &&
		d_media_attr_judge_add(judge, image, bmp, color_palette, media_nai_bmp_color_palette, uint32) &&
		d_media_attr_judge_add(judge, image, bmp, used_color, media_nai_bmp_used_color, uint32)
	) return judge;
	return NULL;
}

// create pri

static void media_container_pri_bmp_free_func(struct media_container_pri_bmp_s *restrict r)
{
	if (r->id) refer_free(r->id);
}

static d_media_container__create_pri(image, bmp)
{
	struct media_container_pri_bmp_s *restrict r;
	if ((r = (struct media_container_pri_bmp_s *) refer_alloz(sizeof(struct media_container_pri_bmp_s))))
	{
		refer_set_free(r, (refer_free_f) media_container_pri_bmp_free_func);
		r->id = (const struct media_container_id_bmp_s *) refer_save(id);
	}
	return r;
}

// parse

static struct media_container_s* media_container_bmp_parse_info_12(struct media_container_s *restrict c)
{
	struct mi_bmp_info_12_t info;
	if (media_io_read(c->io, &info, sizeof(info)) == sizeof(info))
	{
		// media_mlog_print_rawdata(c->media->mlog_verbose, "mi_bmp_info_12_t", &info, sizeof(info));
		if (media_attr_set_int(c->attr, media_nai_width, media_n2le_16(info.width)) &&
			media_attr_set_int(c->attr, media_nai_height, media_n2le_16(info.height)) &&
			media_attr_set_int(c->attr, media_nai_bmp_color_plane, media_n2le_16(info.color_plane)) &&
			media_attr_set_int(c->attr, media_nai_bpp, media_n2le_16(info.bpp))
			) return c;
	}
	return NULL;
}

static struct media_container_s* media_container_bmp_parse_info_40(struct media_container_s *restrict c, struct media_container_pri_bmp_s *restrict pri)
{
	struct mi_bmp_info_40_t info;
	if (media_io_read(c->io, &info, sizeof(info)) == sizeof(info))
	{
		// media_mlog_print_rawdata(c->media->mlog_verbose, "mi_bmp_info_40_t", &info, sizeof(info));
		pri->image_size = media_n2le_32(info.image_size);
		if (media_attr_set_int(c->attr, media_nai_width, media_n2le_32(info.width)) &&
			media_attr_set_int(c->attr, media_nai_height, media_n2le_32(info.height)) &&
			media_attr_set_int(c->attr, media_nai_bmp_color_plane, media_n2le_16(info.color_plane)) &&
			media_attr_set_int(c->attr, media_nai_bpp, media_n2le_16(info.bpp)) &&
			media_attr_set_int(c->attr, media_nai_bmp_compression, media_n2le_32(info.compression_method)) &&
			media_attr_set_int(c->attr, media_nai_bmp_xppm, media_n2le_32(info.xppm)) &&
			media_attr_set_int(c->attr, media_nai_bmp_yppm, media_n2le_32(info.yppm)) &&
			media_attr_set_int(c->attr, media_nai_bmp_color_palette, media_n2le_32(info.color_palette)) &&
			media_attr_set_int(c->attr, media_nai_bmp_used_color, media_n2le_32(info.used_color))
			) return c;
	}
	return NULL;
}

static d_media_container__parse_head(image, bmp)
{
	const struct media_container_id_bmp_s *restrict id;
	const struct media_s *restrict m;
	struct media_io_s *restrict io;
	struct media_container_pri_bmp_s *restrict pri;
	struct mi_bmp_header_t bmp_header;
	refer_string_t magic;
	uint32_t offset, size, version;
	id = (const struct media_container_id_bmp_s *) c->id;
	m = c->media;
	io = c->io;
	pri = (struct media_container_pri_bmp_s *) c->pri_data;
	// get bmp header
	if (media_io_read(io, &bmp_header, sizeof(bmp_header)) != sizeof(bmp_header))
		goto label_fail;
	// media_mlog_print_rawdata(m->mlog_verbose, "bmp-header", &bmp_header, sizeof(bmp_header));
	if (!(magic = (refer_string_t) hashmap_get_head(&id->magic, (uint64_t) bmp_header.magic)))
		goto label_fail;
	if (!media_attr_refer_string(c->attr, media_nas_bmp_magic, magic))
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
	if (!media_attr_set_int(c->attr, media_nai_bmp_version, version))
		goto label_fail;
	// get bmp info
	if ((offset += version) > size)
		goto label_fail;
	switch (version)
	{
		case 12:
			if (media_container_bmp_parse_info_12(c)) break;
			goto label_fail;
		case 40:
			if (media_container_bmp_parse_info_40(c, pri)) break;
			goto label_fail;
		default: goto label_fail;
	}
	// get bits-mask or color-table
	// config stream
	return c;
	label_fail:
	media_warning(m, "image/bmp parse head ... fail");
	return NULL;
}

#define media_container__parse_tail__image__bmp NULL
#define media_container__build_head__image__bmp NULL
#define media_container__build_tail__image__bmp NULL

static struct media_container_id_bmp_s* media_container_id_bmp_initial_magic(struct media_container_id_bmp_s *restrict r, const char *restrict magic)
{
	refer_string_t m;
	if (magic && magic[0] && magic[1] && !magic[2])
	{
		if ((m = refer_dump_string(magic)))
		{
			if (hashmap_set_head(&r->magic, (uint64_t) *(uint16_t *) magic, m, media_hashmap_free_refer_func))
				return r;
			refer_free(m);
		}
	}
	return NULL;
}

static void media_container_id_bmp_free_func(struct media_container_id_bmp_s *restrict r)
{
	hashmap_uini(&r->magic);
	media_container_id_free_func(&r->id);
}

struct media_container_id_s* media_container_create_image_bmp(void)
{
	struct media_container_id_func_t func;
	struct media_container_id_bmp_s *restrict r;
	d_media_container_func_initial(func, image, bmp);
	if ((r = (struct media_container_id_bmp_s *) media_container_id_alloc(sizeof(struct media_container_id_bmp_s), media_nc_bmp, &func)))
	{
		refer_set_free(r, (refer_free_f) media_container_id_bmp_free_func);
		if (hashmap_init(&r->magic) &&
			media_container_id_bmp_initial_magic(r, "BM") && // Windows 3.1x, 95, NT, ... etc.
			media_container_id_bmp_initial_magic(r, "BA") && // OS/2 struct Bitmap Array
			media_container_id_bmp_initial_magic(r, "CI") && // OS/2 struct Color Icon
			media_container_id_bmp_initial_magic(r, "CP") && // OS/2 const Color Pointer
			media_container_id_bmp_initial_magic(r, "IC") && // OS/2 struct Icon
			media_container_id_bmp_initial_magic(r, "PT") && // OS/2 Pointer
			1)
			return &r->id;
		refer_free(r);
	}
	return NULL;
}
