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

static vkaa_std_type_create_define(bytes)
{
	if (!syntax)
		return &vkaa_std_type_bytes_create_by_value(type, NULL)->var;
	return NULL;
}

vkaa_type_s* vkaa_std_tpool_set_bytes(vkaa_tpool_s *restrict tpool, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_tpool_set(tpool, "bytes", typeid->id_bytes, typeid,
		vkaa_std_type_create_label(bytes), NULL);
}
