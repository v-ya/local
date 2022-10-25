#include "container.bmp.h"

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

struct media_container_id_s* media_container_create_image_bmp(const struct media_s *restrict media)
{
	struct media_container_id_func_t func;
	struct media_container_id_bmp_s *restrict r;
	d_media_container_func_initial(func, bmp);
	if ((r = (struct media_container_id_bmp_s *) media_container_id_alloc(sizeof(struct media_container_id_bmp_s), media_nc_bmp, &func)))
	{
		refer_set_free(r, (refer_free_f) media_container_id_bmp_free_func);
		if (hashmap_init(&r->magic) &&
			media_stream_symbol(spec_append, bmp)(r->id.stream_spec, media) &&
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
