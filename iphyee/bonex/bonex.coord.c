#include "bonex.h"

static void iphyee_bonex_coord_free_func(iphyee_bonex_coord_s *restrict r)
{
	if (r->name) refer_free(r->name);
	if (r->base) refer_free(r->base);
}

static iphyee_bonex_coord_s* iphyee_bonex_coord_alloc_empty(void)
{
	iphyee_bonex_coord_s *restrict r;
	uintptr_t default_index;
	if ((r = (iphyee_bonex_coord_s *) refer_alloc(sizeof(iphyee_bonex_coord_s))))
	{
		r->name = r->base = NULL;
		refer_set_free(r, (refer_free_f) iphyee_bonex_coord_free_func);
		r->method = iphyee_bonex_fix_method__e;
		r->property = iphyee_bonex_fix_property__fixed;
		r->bonex_simple_fix = NULL;
		default_index = ~(uintptr_t) 0;
		r->index.base_joint_index = default_index;
		r->index.this_coord_index = default_index;
		r->index.this_value_index = default_index;
		r->index.fix_mat4x4_index = default_index;
		return r;
	}
	return NULL;
}

iphyee_bonex_coord_s* iphyee_bonex_coord_alloc(const iphyee_param_bonex_coord_t *restrict param, const iphyee_bonex_joint_s *restrict base)
{
	iphyee_bonex_coord_s *restrict r;
	if (param && param->this_coord_name && base && (r = iphyee_bonex_coord_alloc_empty()))
	{
		if ((r->name = refer_dump_string(param->this_coord_name)))
		{
			r->base = (refer_string_t) refer_save(base->name);
			r->method = param->method;
			r->property = param->property;
			r->bonex_simple_fix = iphyee_bonex_fix_method(param->method);
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

iphyee_bonex_coord_s* iphyee_bonex_coord_dump(const iphyee_bonex_coord_s *restrict src)
{
	iphyee_bonex_coord_s *restrict r;
	if (src && (r = iphyee_bonex_coord_alloc_empty()))
	{
		r->name = (refer_string_t) refer_save(src->name);
		r->base = (refer_string_t) refer_save(src->base);
		r->method = src->method;
		r->property = src->property;
		r->bonex_simple_fix = src->bonex_simple_fix;
		return r;
	}
	return NULL;
}
