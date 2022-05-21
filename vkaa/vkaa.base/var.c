#include "../vkaa.var.h"
#include "../vkaa.type.h"

vkaa_var_s* vkaa_var_initial(vkaa_var_s *restrict var, const vkaa_type_s *restrict type)
{
	if (type)
	{
		var->type_id = type->id;
		var->type = (const vkaa_type_s *) refer_save(type);
		return var;
	}
	return NULL;
}

void vkaa_var_finally(vkaa_var_s *restrict var)
{
	if (var->type)
	{
		refer_free(var->type);
		var->type = NULL;
	}
}

vkaa_selector_s* vkaa_var_find_selector(const vkaa_var_s *restrict var, const char *restrict name)
{
	return (vkaa_selector_s *) vattr_get_first(var->type->selector, name);
}

static void vkaa_vclear_free_func(vkaa_vclear_s *restrict r)
{
	vkaa_var_s **restrict p;
	vkaa_var_s *restrict v;
	uintptr_t i, n;
	if ((p = r->var_array) && (n = r->var_length))
	{
		for (i = 0; i < n; ++i)
		{
			v = p[i];
			v->type->clear(v);
			refer_free(v);
		}
	}
	exbuffer_uini(&r->buffer);
}

vkaa_vclear_s* vkaa_vclear_alloc(void)
{
	vkaa_vclear_s *restrict r;
	if ((r = (vkaa_vclear_s *) refer_alloz(sizeof(vkaa_vclear_s))))
	{
		if (exbuffer_init(&r->buffer, 0))
		{
			refer_set_free(r, (refer_free_f) vkaa_vclear_free_func);
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

vkaa_vclear_s* vkaa_vclear_push(vkaa_vclear_s *restrict vclear, vkaa_var_s *restrict var)
{
	if (var->type->clear)
	{
		vkaa_var_s **restrict p;
		if ((p = (vkaa_var_s **) exbuffer_need(&vclear->buffer, sizeof(vkaa_var_s *) * (vclear->var_length + 1))))
		{
			p[vclear->var_length] = (vkaa_var_s *) refer_save(var);
			vclear->var_array = p;
			vclear->var_length += 1;
			return vclear;
		}
	}
	return NULL;
}
