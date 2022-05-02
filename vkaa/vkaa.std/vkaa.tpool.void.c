#include "std.tpool.h"

static const char *const s_type_void = "void";

typedef struct vkaa_type_void_s {
	vkaa_type_s type;
} vkaa_type_void_s;

static vkaa_var_s* vkaa_type_void_create_func(const vkaa_type_void_s *restrict type)
{
	vkaa_var_s *restrict r;
	if ((r = (vkaa_var_s *) refer_alloz(sizeof(vkaa_var_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_var_finally);
		if (vkaa_var_initial(r, &type->type, NULL))
			return r;
		refer_free(r);
	}
	return NULL;
}

vkaa_type_s* vkaa_std_type_create_void(const vkaa_tpool_s *restrict tpool, const vkaa_std_typeid_t *restrict typeid)
{
	vkaa_type_void_s *restrict r;
	if ((r = (vkaa_type_void_s *) refer_alloz(sizeof(vkaa_type_void_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_type_finally);
		if (vkaa_type_initial(&r->type, typeid->id_void, s_type_void, (vkaa_type_create_f) vkaa_type_void_create_func))
			return &r->type;
		refer_free(r);
	}
	return NULL;
}
