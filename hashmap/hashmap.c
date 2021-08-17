#include "hashmap.h"
#include <string.h>
#include <stdlib.h>

static uint64_t mix_str(register const char *restrict name)
{
	register uint64_t mix;
	mix = 0;
	while (*name)
	{
		// mix = mix*11 + mix/4 + *name
		mix += (mix<<3) + (mix>>2) + (mix<<1) + *name;
		name++;
	}
	mix ^= mix >> 32;
	mix ^= mix >> 16;
	mix ^= mix >> 8;
	mix ^= mix >> 4;
	mix ^= mix >> 2;
	mix ^= mix >> 1;
	return mix;
}

static uint64_t mix_num(register uint64_t mix)
{
	// mix += (mix<<3) + (mix>>2) + (mix<<1) + mix;
	// mix += (mix<<3) + (mix>>2) + (mix<<1) + mix;
	// mix += (mix<<3) + (mix>>2) + (mix<<1) + mix;
	// mix += (mix<<3) + (mix>>2) + (mix<<1) + mix;
	mix ^= mix >> 32;
	mix ^= mix >> 16;
	mix ^= mix >> 8;
	mix ^= mix >> 4;
	mix ^= mix >> 2;
	mix ^= mix >> 1;
	return mix;
}

// name != NULL => head = mix_str(name)
static hashmap_vlist_t* hashmap_vlist_alloc(register const char *restrict name, register uint64_t head, const void *value, hashmap_func_free_f free_func)
{
	register hashmap_vlist_t *vl;
	if (name)
	{
		uintptr_t n = strlen(name) + 1;
		vl = (hashmap_vlist_t *) malloc(sizeof(hashmap_vlist_t) + n);
		if (vl)
		{
			vl->name = (const char *) memcpy(vl + 1, name, n);
			goto label_okay;
		}
	}
	else
	{
		vl = (hashmap_vlist_t *) malloc(sizeof(hashmap_vlist_t));
		if (vl)
		{
			vl->name = NULL;
			label_okay:
			vl->head = head;
			vl->value = (void *) value;
			vl->free = free_func;
			vl->next = NULL;
			return vl;
		}
	}
	return NULL;
}

static inline void hashmap_vlist_free(register hashmap_vlist_t *restrict vl)
{
	if (vl->free) vl->free(vl);
	free(vl);
}

static void hashmap_vlist_listfree(register hashmap_vlist_t *restrict vl)
{
	register hashmap_vlist_t *vb;
	while (vl)
	{
		vb = vl;
		vl = vl->next;
		hashmap_vlist_free(vb);
	}
}

static inline void hashmap_vlist_insert(register hashmap_vlist_t *restrict *restrict vlp, register hashmap_vlist_t *restrict vl)
{
	vl->next = *vlp;
	*vlp = vl;
}

// name != NULL => head = mix_str(name)
static hashmap_vlist_t* hashmap_vlist_find(register const hashmap_vlist_t *restrict vl, register const char *restrict name, register uint64_t head)
{
	if (name) while (vl)
	{
		if (vl->name && vl->head == head && !strcmp(vl->name, name)) return (hashmap_vlist_t *) vl;
		vl = vl->next;
	}
	else while (vl)
	{
		if (!vl->name && vl->head == head) return (hashmap_vlist_t *) vl;
		vl = vl->next;
	}
	return NULL;
}

// name != NULL => head = mix_str(name)
static void* hashmap_vlist_delete(register hashmap_vlist_t *restrict *restrict vlp, register const char *restrict name, register uint64_t head)
{
	register hashmap_vlist_t *vl;
	if (name) while ((vl = *vlp))
	{
		if (vl->name && vl->head == head && !strcmp(vl->name, name))
		{
			*vlp = vl->next;
			hashmap_vlist_free(vl);
			return vl;
		}
		vlp = &vl->next;
	}
	else while ((vl = *vlp))
	{
		if (!vl->name && vl->head == head)
		{
			*vlp = vl->next;
			hashmap_vlist_free(vl);
			return vl;
		}
		vlp = &vl->next;
	}
	return NULL;
}

hashmap_t* hashmap_init(register hashmap_t *restrict hm)
{
	hm->mask = hashmap_init_size - 1;
	hm->number = 0;
	hm->map = (hashmap_vlist_t **) calloc(hashmap_init_size, sizeof(hashmap_vlist_t *));
	if (hm->map) return hm;
	return NULL;
}

void hashmap_uini(register hashmap_t *restrict hm)
{
	register hashmap_vlist_t **map;
	map = hm->map;
	if (map)
	{
		register uint32_t i;
		i = hm->mask + 1;
		while (i)
		{
			--i;
			hashmap_vlist_listfree(map[i]);
		}
		free(map);
		hm->map = NULL;
	}
}

void hashmap_clear(register hashmap_t *restrict hm)
{
	register hashmap_vlist_t **map;
	map = hm->map;
	if (map)
	{
		register uint32_t i;
		i = hm->mask + 1;
		while (i)
		{
			--i;
			hashmap_vlist_listfree(map[i]);
		}
		bzero(map, (hm->mask + 1) * sizeof(hashmap_vlist_t *));
		hm->number = 0;
	}
}

hashmap_t* hashmap_alloc(void)
{
	register hashmap_t *hm;
	hm = (hashmap_t *) malloc(sizeof(hashmap_t));
	if (hm)
	{
		if (hashmap_init(hm)) return hm;
		else free(hm);
	}
	return NULL;
}

void hashmap_free(register hashmap_t *restrict hm)
{
	hashmap_uini(hm);
	free(hm);
}

static void hashmap_exmap(register hashmap_t *restrict hm)
{
	register uint32_t i, mask;
	register hashmap_vlist_t *vl, *vb;
	hashmap_vlist_t **map;
	mask = hm->mask;
	map = hm->map;
	if ((hm->map = (hashmap_vlist_t**)calloc((mask + 1) << 2, sizeof(hashmap_vlist_t*))))
	{
		i = mask + 1;
		hm->mask = mask = (mask << 1) | 1;
		while (i)
		{
			vl = map[--i];
			while (vl)
			{
				vb = vl;
				vl = vl->next;
				hashmap_vlist_insert(hm->map + ((vb->name?vb->head:mix_num(vb->head)) & mask), vb);
			}
		}
		free(map);
	}
	else hm->map = map;
}

static inline void hashmap_insert(register hashmap_t *restrict hm, register hashmap_vlist_t *restrict vl)
{
	register uint32_t mask = hm->mask;
	hashmap_vlist_insert(hm->map + ((vl->name?vl->head:mix_num(vl->head)) & mask), vl);
	if (++(hm->number) > mask && !(mask & 0x80000000)) hashmap_exmap(hm);
}

// hm != NULL, name != NULL => head = mix_str(name)
static inline hashmap_vlist_t* hashmap_find(register const hashmap_t *restrict hm, register const char *restrict name, register uint64_t head)
{
	return hashmap_vlist_find(hm->map[(name?head:mix_num(head)) & hm->mask], name, head);
}

hashmap_vlist_t* hashmap_find_name(register const hashmap_t *restrict hm, register const char *restrict name)
{
	register uint64_t head;
	head = mix_str(name);
	return hashmap_vlist_find(hm->map[head & hm->mask], name, head);
}

hashmap_vlist_t* hashmap_find_head(register const hashmap_t *restrict hm, register uint64_t head)
{
	return hashmap_vlist_find(hm->map[mix_num(head) & hm->mask], NULL, head);
}

void hashmap_delete_name(register hashmap_t *restrict hm, register const char *restrict name)
{
	register uint64_t head;
	head = mix_str(name);
	if (hashmap_vlist_delete(hm->map + (head & hm->mask), name, head))
		hm->number--;
}

void hashmap_delete_head(register hashmap_t *restrict hm, register uint64_t head)
{
	if (hashmap_vlist_delete(hm->map + (mix_num(head) & hm->mask), NULL, head))
		hm->number--;
}

hashmap_vlist_t* hashmap_put_name(register hashmap_t *restrict hm, register const char *restrict name, const void *value, hashmap_func_free_f free_func)
{
	register hashmap_vlist_t *vl;
	register uint64_t head;
	head = mix_str(name);
	vl = hashmap_find(hm, name, head);
	if (vl) return vl;
	vl = hashmap_vlist_alloc(name, head, value, free_func);
	if (!vl) return NULL;
	hashmap_insert(hm, vl);
	return vl;
}

hashmap_vlist_t* hashmap_put_head(register hashmap_t *restrict hm, register uint64_t head, const void *value, hashmap_func_free_f free_func)
{
	register hashmap_vlist_t *vl;
	vl = hashmap_find_head(hm, head);
	if (vl) return vl;
	vl = hashmap_vlist_alloc(NULL, head, value, free_func);
	if (!vl) return NULL;
	hashmap_insert(hm, vl);
	return vl;
}

hashmap_vlist_t* hashmap_set_name(register hashmap_t *restrict hm, register const char *restrict name, const void *value, hashmap_func_free_f free_func)
{
	register hashmap_vlist_t *vl;
	register uint64_t head;
	head = mix_str(name);
	vl = hashmap_find(hm, name, head);
	if (vl)
	{
		if (vl->free) vl->free(vl);
		vl->value = (void *) value;
		vl->free = free_func;
		return vl;
	}
	else
	{
		vl = hashmap_vlist_alloc(name, head, value, free_func);
		if (!vl) return NULL;
		hashmap_insert(hm, vl);
		return vl;
	}
}

hashmap_vlist_t* hashmap_set_head(register hashmap_t *restrict hm, register uint64_t head, const void *value, hashmap_func_free_f free_func)
{
	register hashmap_vlist_t *vl;
	vl = hashmap_find_head(hm, head);
	if (vl)
	{
		if (vl->free) vl->free(vl);
		vl->value = (void *) value;
		vl->free = free_func;
		return vl;
	}
	else
	{
		vl = hashmap_vlist_alloc(NULL, head, value, free_func);
		if (!vl) return NULL;
		hashmap_insert(hm, vl);
		return vl;
	}
}

void* hashmap_get_name(register const hashmap_t *restrict hm, register const char *restrict name)
{
	register hashmap_vlist_t *vl;
	vl = hashmap_find(hm, name, mix_str(name));
	if (vl) return vl->value;
	else return NULL;
}

void* hashmap_get_head(register const hashmap_t *restrict hm, register uint64_t head)
{
	register hashmap_vlist_t *vl;
	vl = hashmap_find(hm, NULL, head);
	if (vl) return vl->value;
	else return NULL;
}

void hashmap_call(register const hashmap_t *restrict hm, register hashmap_func_call_f callFunc, const void *data)
{
	register hashmap_vlist_t *vl;
	if (hm->map)
	{
		register uint32_t i;
		i = hm->mask + 1;
		do {
			--i;
			vl = hm->map[i];
			while(vl)
			{
				callFunc(vl, (void *) data);
				vl = vl->next;
			}
		}
		while (i);
	}
}

void hashmap_call_v2(register const hashmap_t *restrict hm, register hashmap_func_call_v2_f callFunc, const void *v1, const void *v2)
{
	register hashmap_vlist_t *vl;
	if (hm->map)
	{
		register uint32_t i;
		i = hm->mask + 1;
		do {
			--i;
			vl = hm->map[i];
			while(vl)
			{
				callFunc(vl, (void *) v1, (void *) v2);
				vl = vl->next;
			}
		}
		while (i);
	}
}

void hashmap_isfree(register hashmap_t *restrict hm, register hashmap_func_isfree_f isfree_func, const void *data)
{
	register hashmap_vlist_t *vl, **pvl;
	if (hm->map)
	{
		register uint32_t i;
		i = hm->mask + 1;
		do {
			--i;
			pvl = hm->map + i;
			while((vl = *pvl))
			{
				if (isfree_func(vl, (void *) data))
				{
					// free
					*pvl = vl->next;
					hashmap_vlist_free(vl);
					--hm->number;
				}
				else
				{
					// no-free
					pvl = &vl->next;
				}
			}
		}
		while (i);
	}
}
