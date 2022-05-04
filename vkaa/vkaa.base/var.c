#include "../vkaa.var.h"
#include "../vkaa.type.h"

vkaa_var_s* vkaa_var_initial(vkaa_var_s *restrict var, const vkaa_type_s *restrict type, void *value)
{
	if (type)
	{
		var->type_id = type->id;
		var->type = (const vkaa_type_s *) refer_save(type);
		var->value = value;
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
