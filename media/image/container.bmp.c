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
};

struct media_container_id_bmp_magic_s {
	refer_string_t magic;
};

struct mi_bmp_header_t {
	uint16_t magic;
	uint32_t bmp_file_size;
	uint16_t reserve1;
	uint16_t reserve2;
	uint32_t pixel_offset;
} __attribute__ ((packed));

// judge

static d_media_judge(image, bmp, magic, struct media_container_pri_bmp_s)
{
	const char *restrict s;
	uint16_t magic;
	if (!value)
		pri->magic = *(uint16_t *) "BM";
	else if (value->type == media_attr_type__string)
	{
		s = value->value.av_string;
		if (!s || !s[0] || !s[1] || s[2])
			goto label_fail;
		magic = *(uint16_t *) s;
		if (!hashmap_get_head(&pri->id->magic, (uint64_t) magic))
			goto label_fail;
		pri->magic = magic;
	}
	else goto label_fail;
	return attr;
	label_fail:
	return NULL;
}
static d_media_judge(image, bmp, clear, struct media_container_pri_bmp_s)
{
	if (!value)
	{
		media_attr_judge(image, bmp, magic)(attr, pri, NULL);
		return attr;
	}
	return NULL;
}

static d_media_container__initial_judge(image, bmp)
{
	if (media_attr_judge_set(judge, NULL, (media_attr_judge_f) media_attr_judge(image, bmp, clear)) &&
		media_attr_judge_set(judge, media_nas_bmp_magic, (media_attr_judge_f) media_attr_judge(image, bmp, magic))
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

static d_media_container__parse_head(image, bmp)
{
	const struct media_container_id_bmp_s *restrict id;
	const struct media_s *restrict m;
	struct media_io_s *restrict io;
	struct mi_bmp_header_t bmp_header;
	struct media_container_id_bmp_magic_s *restrict magic;
	union media_attr_value_t value;
	// uint32_t offset, size, pixel_offset;
	id = (const struct media_container_id_bmp_s *) c->id;
	m = c->media;
	io = c->io;
	media_verbose(m, "image/bmp parse head ...");
	if (media_io_read(io, &bmp_header, sizeof(bmp_header)) != sizeof(bmp_header))
		goto label_fail;
	// offset = sizeof(bmp_header);
	// size = media_n2le_32(bmp_header.bmp_file_size);
	// pixel_offset = media_n2le_32(bmp_header.pixel_offset);
	if (!(magic = (struct media_container_id_bmp_magic_s *) hashmap_get_head(&id->magic, (uint64_t) bmp_header.magic)))
		goto label_fail;
	// value.av_string = magic->magic;
	if (!media_attr_set(c->attr, media_nas_bmp_magic, media_attr_type__string, value))
		goto label_fail;
	return c;
	label_fail:
	media_warning(m, "image/bmp parse head ... fail");
	return NULL;
}

#define media_container__parse_tail__image__bmp NULL
#define media_container__build_head__image__bmp NULL
#define media_container__build_tail__image__bmp NULL

static void media_container_id_bmp_magic_free_func(struct media_container_id_bmp_magic_s *restrict r)
{
	if (r->magic) refer_free(r->magic);
}

static struct media_container_id_bmp_s* media_container_id_bmp_initial_magic(struct media_container_id_bmp_s *restrict r, const char *restrict magic)
{
	struct media_container_id_bmp_magic_s *restrict m;
	if (magic && magic[0] && magic[1] && !magic[2])
	{
		if ((m = (struct media_container_id_bmp_magic_s *) refer_alloz(sizeof(struct media_container_id_bmp_magic_s))))
		{
			refer_set_free(m, (refer_free_f) media_container_id_bmp_magic_free_func);
			if ((m->magic = refer_dump_string(magic)))
			{
				if (hashmap_set_head(&r->magic, (uint64_t) *(uint16_t *) magic, m, media_container_id_hashmap_free_func))
					return r;
			}
			refer_free(r);
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
