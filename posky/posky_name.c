#include "posky_name.h"
#include <stddef.h>

hashmap_t* posky_create_name(hashmap_t *restrict name_pool, const char *restrict name, uint64_t address, const void *v)
{
	hashmap_vlist_t *restrict vl;
	vl = hashmap_put_name(name_pool, name, v, NULL);
	if (vl && vl->value == v)
	{
		if (hashmap_set_head(name_pool, address, vl->name, NULL))
			return name_pool;
		hashmap_delete_name(name_pool, name);
	}
	return NULL;
}

void posky_delete_name(hashmap_t *restrict group_name, uint64_t address)
{
	const char *restrict name;
	if ((name = (const char *) hashmap_get_head(group_name, address)))
	{
		hashmap_delete_head(group_name, address);
		hashmap_delete_name(group_name, name);
	}
}
