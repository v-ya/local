#include "layer.model.h"
#include <rbtree.h>
#include <hashmap.h>

struct layer_model_enum_s {
	hashmap_t s2u;
	rbtree_t *u2s;
	uint64_t dv;
};

static void layer_model_enum_free_func(layer_model_enum_s *restrict r)
{
	rbtree_clear(&r->u2s);
	hashmap_uini(&r->s2u);
}

layer_model_enum_s* layer_model_enum_alloc(uint64_t dv)
{
	layer_model_enum_s *restrict r;
	if ((r = (layer_model_enum_s *) refer_alloz(sizeof(layer_model_enum_s))))
	{
		refer_set_free(r, (refer_free_f) layer_model_enum_free_func);
		if (hashmap_init(&r->s2u))
		{
			r->dv = dv;
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

layer_model_enum_s* layer_model_enum_insert(layer_model_enum_s *restrict e, const char *restrict name, uint64_t value)
{
	hashmap_vlist_t *restrict vl;
	rbtree_t *restrict rbv;
	if (name && !hashmap_find_name(&e->s2u, name) && !rbtree_find(&e->u2s, NULL, value))
	{
		if ((vl = hashmap_set_name(&e->s2u, name, NULL, NULL)))
		{
			if ((rbv = rbtree_insert(&e->u2s, NULL, value, vl->name, NULL)))
			{
				vl->value = (void *) &rbv->key_index;
				return e;
			}
			hashmap_delete_name(&e->s2u, name);
		}
	}
	return NULL;
}

uint64_t layer_model_enum_dv(const layer_model_enum_s *restrict e)
{
	return e->dv;
}

const char* layer_model_enum_name(const layer_model_enum_s *restrict e, uint64_t value)
{
	rbtree_t *restrict rbv;
	if ((rbv = rbtree_find(&e->u2s, NULL, value)))
		return (const char *) rbv->value;
	return NULL;
}

const layer_model_enum_s* layer_model_enum_value(const layer_model_enum_s *restrict e, const char *restrict name, uint64_t *restrict value)
{
	const uint64_t *restrict pv;
	if ((pv = (const uint64_t *) hashmap_get_name(&e->s2u, name)))
	{
		if (value) *value = *pv;
		return e;
	}
	return NULL;
}
