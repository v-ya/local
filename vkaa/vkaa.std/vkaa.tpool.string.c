#include "std.tpool.h"

static void vkaa_std_var_string_free_func(vkaa_std_var_string_s *restrict r)
{
	if (r->value) refer_free(r->value);
	vkaa_var_finally(&r->var);
}

vkaa_std_var_string_s* vkaa_std_type_string_create_by_value(const vkaa_type_s *restrict type, refer_nstring_t value)
{
	vkaa_std_var_string_s *restrict r;
	if ((r = (vkaa_std_var_string_s *) refer_alloz(sizeof(vkaa_std_var_string_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_std_var_string_free_func);
		if (vkaa_var_initial(&r->var, type))
		{
			r->value = (refer_nstring_t) refer_save(value);
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

void vkaa_std_var_string_mov(vkaa_std_var_string_s *restrict dst, const vkaa_std_var_string_s *restrict src)
{
	refer_nstring_t value;
	refer_save(value = src->value);
	if (dst->value) refer_free(dst->value);
	dst->value = value;
}

static vkaa_std_type_create_define(string)
{
	if (!syntax)
		return &vkaa_std_type_string_create_by_value(type, NULL)->var;
	if (syntax->syntax_number == 1)
	{
		const vkaa_syntax_t *restrict s;
		s = syntax->syntax_array;
		if (s->type == vkaa_syntax_type_string)
			return &vkaa_std_type_string_create_by_value(type, s->data.string)->var;
	}
	return NULL;
}

static vkaa_std_type_init_define(string)
{
	if (
		vkaa_std_type_set_function_si(type, "=", sfsi_need2m(string, op_mov)) &&
	1) return type;
	return NULL;
}

vkaa_type_s* vkaa_std_tpool_set_string(vkaa_tpool_s *restrict tpool, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_tpool_set(tpool, "string", typeid->id_string, typeid,
		vkaa_std_type_create_label(string), vkaa_std_type_init_label(string));
}
