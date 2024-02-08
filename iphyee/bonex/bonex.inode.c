#include "bonex.h"

static void iphyee_bonex_inode_free_func(iphyee_bonex_inode_s *restrict r)
{
	if (r->name) refer_free(r->name);
	if (r->base) refer_free(r->base);
	if (r->bonex_complex_fix) refer_free(r->bonex_complex_fix);
}

static iphyee_bonex_inode_s* iphyee_bonex_inode_alloc_empty(void)
{
	iphyee_bonex_inode_s *restrict r;
	uintptr_t default_index;
	if ((r = (iphyee_bonex_inode_s *) refer_alloc(sizeof(iphyee_bonex_inode_s))))
	{
		r->name = r->base = NULL;
		r->bonex_complex_fix = NULL;
		refer_set_free(r, (refer_free_f) iphyee_bonex_inode_free_func);
		r->method = iphyee_bonex_fix_method__e;
		r->property = iphyee_bonex_fix_property__fixed;
		r->bonex_simple_fix = NULL;
		r->default_value_minimum = 0;
		r->default_value_maximum = 0;
		r->default_value_multiplier = 0;
		r->default_value_addend = 0;
		default_index = ~(uintptr_t) 0;
		r->index.base_joint_index = default_index;
		r->index.this_inode_index = default_index;
		r->index.this_value_index = default_index;
		r->index.fix_mat4x4_index = default_index;
		return r;
	}
	return NULL;
}

iphyee_bonex_inode_s* iphyee_bonex_inode_alloc(const iphyee_param_bonex_inode_t *restrict param, const iphyee_bonex_joint_s *restrict base)
{
	iphyee_bonex_inode_s *restrict r;
	if (param && param->this_inode_name && base && (r = iphyee_bonex_inode_alloc_empty()))
	{
		if ((r->name = refer_dump_string(param->this_inode_name)))
		{
			r->base = (refer_string_t) refer_save(base->name);
			r->method = param->method;
			r->property = param->property;
			r->bonex_simple_fix = iphyee_bonex_fix_method(param->method);
			if (r->method == iphyee_bonex_fix_method__custom)
				r->bonex_complex_fix = (iphyee_bonex_complex_s *) refer_save(param->fix_custom_complex);
			r->default_value_minimum = param->default_value_minimum;
			r->default_value_maximum = param->default_value_maximum;
			r->default_value_multiplier = param->default_value_multiplier;
			r->default_value_addend = param->default_value_addend;
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

iphyee_bonex_inode_s* iphyee_bonex_inode_dump(const iphyee_bonex_inode_s *restrict src)
{
	iphyee_bonex_inode_s *restrict r;
	if (src && (r = iphyee_bonex_inode_alloc_empty()))
	{
		r->name = (refer_string_t) refer_save(src->name);
		r->base = (refer_string_t) refer_save(src->base);
		r->method = src->method;
		r->property = src->property;
		r->bonex_simple_fix = src->bonex_simple_fix;
		r->bonex_complex_fix = (iphyee_bonex_complex_s *) refer_save(src->bonex_complex_fix);
		r->default_value_minimum = src->default_value_minimum;
		r->default_value_maximum = src->default_value_maximum;
		r->default_value_multiplier = src->default_value_multiplier;
		r->default_value_addend = src->default_value_addend;
		return r;
	}
	return NULL;
}
