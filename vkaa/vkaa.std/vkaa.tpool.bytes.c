#include "std.tpool.h"
#include <memory.h>

static void vkaa_std_var_bytes_value_free_func(vkaa_std_var_bytes_value_s *restrict r)
{
	exbuffer_uini(&r->bytes);
}

static vkaa_std_var_bytes_value_s* vkaa_std_var_bytes_value_alloc(uintptr_t need_size)
{
	vkaa_std_var_bytes_value_s *restrict r;
	if ((r = (vkaa_std_var_bytes_value_s *) refer_alloc(sizeof(vkaa_std_var_bytes_value_s))))
	{
		if (exbuffer_init(&r->bytes, need_size))
		{
			refer_set_free(r, (refer_free_f) vkaa_std_var_bytes_value_free_func);
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

static void vkaa_std_var_bytes_free_func(vkaa_std_var_bytes_s *restrict r)
{
	if (r->value) refer_free(r->value);
	vkaa_var_finally(&r->var);
}

vkaa_std_var_bytes_s* vkaa_std_type_bytes_create_by_value(const vkaa_type_s *restrict type, vkaa_std_var_bytes_value_s *restrict value)
{
	vkaa_std_var_bytes_s *restrict r;
	if ((r = (vkaa_std_var_bytes_s *) refer_alloz(sizeof(vkaa_std_var_bytes_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_std_var_bytes_free_func);
		if (vkaa_var_initial(&r->var, type))
		{
			r->value = (vkaa_std_var_bytes_value_s *) refer_save(value);
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

void vkaa_std_var_bytes_clear(vkaa_std_var_bytes_s *restrict var)
{
	if (var->value)
	{
		refer_free(var->value);
		var->value = NULL;
	}
}

void vkaa_std_var_bytes_mov(vkaa_std_var_bytes_s *restrict dst, const vkaa_std_var_bytes_s *restrict src)
{
	vkaa_std_var_bytes_value_s *restrict value;
	refer_save(value = src->value);
	if (dst->value) refer_free(dst->value);
	dst->value = value;
}

vkaa_std_var_bytes_value_s* vkaa_std_var_bytes_value(vkaa_std_var_bytes_s *restrict var, uintptr_t need_size)
{
	if (var->value)
		return var->value;
	return (var->value = vkaa_std_var_bytes_value_alloc(need_size));
}

vkaa_std_var_bytes_value_s* vkaa_std_var_bytes_append(vkaa_std_var_bytes_s *restrict var, const void *restrict data, uintptr_t size)
{
	vkaa_std_var_bytes_value_s *restrict v;
	if ((v = var->value) || (var->value = v = vkaa_std_var_bytes_value_alloc(size)))
	{
		if (exbuffer_append(&v->bytes, data, size))
			return v;
	}
	return NULL;
}

static vkaa_std_type_create_define(bytes)
{
	if (!syntax)
		return &vkaa_std_type_bytes_create_by_value(type, NULL)->var;
	return NULL;
}

static vkaa_std_type_init_define(bytes)
{
	const vkaa_std_selector_desc_input_t fn_append_maybe[] = {
		{.type_enum_number = 2, .type_enum_array = (const uintptr_t []) {typeid->id_string, typeid->id_bytes}},
		{.type_enum_number = 0, .type_enum_array = NULL}
	};
	if (
		vkaa_std_type_set_function_si(type, "=", sfsi_need2m(bytes, op_mov)) &&
		vkaa_std_type_set_function(type, "string", sf_need0cvn(bytes, string, cv_string)) &&
		vkaa_std_type_set_function(type, "clear", vkaa_std_function_label(bytes, fn_clear), NULL,
			vkaa_std_selector_output_this, vkaa_std_selector_convert_none,
			typeid->id_bytes, typeid->id_bytes, 0, NULL) &&
		vkaa_std_type_set_function_any(type, "append", vkaa_std_function_label(bytes, fn_append), typeid,
			vkaa_std_selector_output_this, vkaa_std_selector_convert_none,
			typeid->id_bytes, typeid->id_bytes, NULL, fn_append_maybe) &&
	1) return type;
	return NULL;
}

vkaa_type_s* vkaa_std_tpool_set_bytes(vkaa_tpool_s *restrict tpool, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_tpool_set(tpool, "bytes", typeid->id_bytes, typeid,
		vkaa_std_type_create_label(bytes), vkaa_std_type_init_label(bytes));
}
