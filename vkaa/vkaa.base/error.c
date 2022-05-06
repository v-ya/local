#include "../vkaa.error.h"

static void vkaa_error_free_func(vkaa_error_s *restrict r)
{
	hashmap_uini(&r->n2i);
	hashmap_uini(&r->i2n);
}

vkaa_error_s* vkaa_error_alloc(void)
{
	vkaa_error_s *restrict r;
	if ((r = (vkaa_error_s *) refer_alloz(sizeof(vkaa_error_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_error_free_func);
		if (hashmap_init(&r->n2i) && hashmap_init(&r->i2n))
			return r;
		refer_free(r);
	}
	return NULL;
}

vkaa_error_s* vkaa_error_add_error(vkaa_error_s *restrict error, const char *restrict name)
{
	hashmap_vlist_t *restrict vl;
	uintptr_t id;
	id = (uintptr_t) error->i2n.number + 2;
	if (name && !hashmap_find_name(&error->n2i, name) &&
		!hashmap_find_head(&error->i2n, (uint64_t) id))
	{
		if ((vl = hashmap_set_name(&error->n2i, name, (void *) id, NULL)))
		{
			if (hashmap_set_head(&error->i2n, (uint64_t) id, vl->name, NULL))
				return error;
			hashmap_delete_name(&error->n2i, name);
		}
	}
	return NULL;
}

uintptr_t vkaa_error_get_id(vkaa_error_s *restrict error, const char *restrict name)
{
	return (uintptr_t) hashmap_get_name(&error->n2i, name) - 1;
}

const char* vkaa_error_get_name(vkaa_error_s *restrict error, uintptr_t id)
{
	return (const char *) hashmap_get_head(&error->i2n, (uint64_t) (id + 1));
}
