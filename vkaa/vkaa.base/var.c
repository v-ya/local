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
	rbtree_clear(&r->var);
}

static void vkaa_vclear_var_free_func(rbtree_t *restrict rbv)
{
	vkaa_var_s *restrict var;
	if ((var = rbv->value))
	{
		var->type->clear(var);
		refer_free(var);
	}
}

vkaa_vclear_s* vkaa_vclear_alloc(void)
{
	vkaa_vclear_s *restrict r;
	if ((r = (vkaa_vclear_s *) refer_alloz(sizeof(vkaa_vclear_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_vclear_free_func);
		return r;
	}
	return NULL;
}

vkaa_vclear_s* vkaa_vclear_push(vkaa_vclear_s *restrict vclear, vkaa_var_s *restrict var)
{
	if (var->type->clear)
	{
		if (rbtree_find(&vclear->var, NULL, (uint64_t) (uintptr_t) var))
			goto label_okay;
		else if (rbtree_insert(&vclear->var, NULL, (uint64_t) (uintptr_t) var, var, vkaa_vclear_var_free_func))
		{
			refer_save(var);
			label_okay:
			return vclear;
		}
	}
	return NULL;
}
