#ifndef _posky_name_h_
#define _posky_name_h_

#include <hashmap.h>

hashmap_t* posky_create_name(hashmap_t *restrict name_pool, const char *restrict name, uint64_t address, const void *v);
void posky_delete_name(hashmap_t *restrict group_name, uint64_t address);

#endif
