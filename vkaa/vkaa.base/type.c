#include "../vkaa.type.h"

vkaa_type_s* vkaa_type_initial(vkaa_type_s *restrict type, uintptr_t id, const char *name, vkaa_type_create_f create)
{
	if (create && !type->selector && (type->selector = vattr_alloc()))
	{
		type->id = id;
		type->name = name;
		type->error = NULL;
		type->create = create;
		type->clear = NULL;
		return type;
	}
	return NULL;
}

void vkaa_type_finally(vkaa_type_s *restrict type)
{
	if (type->selector)
	{
		refer_free(type->selector);
		type->selector = NULL;
	}
	if (type->error)
	{
		refer_free(type->error);
		type->error = NULL;
	}
}

vkaa_selector_s* vkaa_type_find_selector(const vkaa_type_s *restrict type, const char *restrict name)
{
	return (vkaa_selector_s *) vattr_get_first(type->selector, name);
}

vkaa_selector_s* vkaa_type_insert_selector(const vkaa_type_s *restrict type, const char *restrict name, vkaa_selector_s *restrict selector)
{
	if (selector && vattr_insert_first(type->selector, name, selector))
		return selector;
	return NULL;
}
