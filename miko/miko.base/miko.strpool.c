#include "miko.strpool.h"
#include <string.h>
#include <stdlib.h>

static void miko_strpool_item_free_func(hashmap_vlist_t *restrict vl)
{
	miko_strpool_item_t *restrict r;
	if ((r = (miko_strpool_item_t *) vl->value))
	{
		refer_ck_free(r->string);
		free(r);
	}
}

static void miko_strpool_free_func(miko_strpool_s *restrict r)
{
	hashmap_uini(&r->strpool);
}

miko_strpool_s_t miko_strpool_alloc(void)
{
	miko_strpool_s *restrict r;
	if ((r = (miko_strpool_s *) refer_alloz(sizeof(miko_strpool_s))))
	{
		refer_hook_free(r, strpool);
		if (hashmap_init(&r->strpool))
			return r;
		refer_free(r);
	}
	return NULL;
}

refer_string_t miko_strpool_save(miko_strpool_s_t r, const char *restrict name)
{
	miko_strpool_item_t *restrict v;
	if (name)
	{
		if ((v = (miko_strpool_item_t *) hashmap_get_name(&r->strpool, name)))
		{
			label_find:
			v->count += 1;
			return (refer_string_t) refer_save(v->string);
		}
		else if ((v = (miko_strpool_item_t *) malloc(sizeof(miko_strpool_item_t))))
		{
			v->count = 0;
			v->length = strlen(name);
			if ((v->string = refer_dump_string_with_length(name, v->length)))
			{
				if (hashmap_set_name(&r->strpool, name, v, miko_strpool_item_free_func))
					goto label_find;
				refer_free(v->string);
			}
			free(v);
		}
	}
	return NULL;
}

void miko_strpool_undo(miko_strpool_s_t r, const char *restrict name)
{
	miko_strpool_item_t *restrict v;
	if (name && (v = (miko_strpool_item_t *) hashmap_get_name(&r->strpool, name)) && !(v->count -= 1))
		hashmap_delete_name(&r->strpool, name);
}
