#include "../vkaa.var.h"
#include "../vkaa.type.h"

vkaa_var_s* vkaa_var_initial(vkaa_var_s *restrict var, const vkaa_type_s *restrict type, hashmap_t *restrict selector, void *value)
{
	if (type && !var->selector && (!selector || hashmap_init(selector)))
	{
		var->type_id = type->id;
		var->type = (const vkaa_type_s *) refer_save(type);
		var->selector = selector;
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
	if (var->selector)
	{
		hashmap_uini(var->selector);
		var->selector = NULL;
	}
}
