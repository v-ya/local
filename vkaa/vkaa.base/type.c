#include "../vkaa.type.h"

vkaa_type_s* vkaa_type_initial(vkaa_type_s *restrict type, uintptr_t id, const char *name, vkaa_type_create_f create)
{
	if (create && !type->selector && (type->selector = vattr_alloc()))
	{
		type->id = id;
		type->name = name;
		type->create = create;
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
}