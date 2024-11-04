#include "miko.strpool.h"
#include <rbtree.h>
#include <string.h>
#include <stdlib.h>

// strpool

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

refer_string_t miko_strpool_fetch(miko_strpool_s_t r, const char *restrict name)
{
	miko_strpool_item_t *restrict v;
	if (name)
	{
		if ((v = (miko_strpool_item_t *) hashmap_get_name(&r->strpool, name)))
		{
			label_find:
			v->count += 1;
			return v->string;
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

refer_string_t miko_strpool_save(miko_strpool_s_t r, const char *restrict name)
{
	return (refer_string_t) refer_save(miko_strpool_fetch(r, name));
}

void miko_strpool_undo(miko_strpool_s_t r, const char *restrict name)
{
	miko_strpool_item_t *restrict v;
	if (name && (v = (miko_strpool_item_t *) hashmap_get_name(&r->strpool, name)) && !(v->count -= 1))
		hashmap_delete_name(&r->strpool, name);
}

// sort and create strlist

static void miko_strpool_sort_free_func(rbtree_t *restrict rbv)
{
	rbtree_clear((rbtree_t **) &rbv->value);
}

static void miko_strpool_sort_call_func(hashmap_vlist_t *restrict vl, rbtree_t *restrict *restrict *restrict pp_sort)
{
	const miko_strpool_item_t *restrict item;
	rbtree_t *restrict rbv;
	if (*pp_sort)
	{
		item = (const miko_strpool_item_t *) vl->value;
		if ((rbv = rbtree_find(*pp_sort, NULL, (uint64_t) item->count)))
		{
			label_find:
			if (rbtree_insert((rbtree_t **) &rbv->value, NULL, (uint64_t) (uintptr_t) item, item, NULL))
				return ;
		}
		else if ((rbv = rbtree_insert(*pp_sort, NULL, (uint64_t) item->count, NULL, miko_strpool_sort_free_func)))
			goto label_find;
		// fail
		*pp_sort = NULL;
	}
}

miko_strlist_s_t miko_strpool_create_strlist(miko_strpool_s_t r)
{
	const miko_strpool_item_t *restrict item;
	miko_strlist_s *restrict strlist;
	rbtree_t *restrict iter_rbtree;
	rbtree_t *restrict iter_item;
	rbtree_t *sort;
	rbtree_t **psort;
	miko_strlist_s_t result;
	result = NULL;
	sort = NULL;
	psort = &sort;
	hashmap_call(&r->strpool, (hashmap_func_call_f) miko_strpool_sort_call_func, (const void *) &psort);
	if (*psort && (strlist = miko_strlist_alloc()))
	{
		if (!miko_strlist_push(strlist, NULL, 0))
			goto label_fail;
		for (iter_rbtree = rbtree_tail(&sort); iter_rbtree; iter_rbtree = rbtree_last(iter_rbtree))
		{
			for (iter_item = rbtree_first((rbtree_t **) &iter_rbtree->value); iter_item; iter_item = rbtree_next(iter_item))
			{
				item = (const miko_strpool_item_t *) iter_item->value;
				if (!miko_strlist_push(strlist, item->string, item->length))
					goto label_fail;
			}
		}
		result = strlist;
		strlist = NULL;
		label_fail:
		refer_ck_free(strlist);
	}
	rbtree_clear(&sort);
	return result;
}

// strlist

static void miko_strlist_free_func(miko_strlist_s *restrict r)
{
	hashmap_uini(&r->strlist);
	exbuffer_uini(&r->data);
}

miko_strlist_s* miko_strlist_alloc(void)
{
	miko_strlist_s *restrict r;
	if ((r = (miko_strlist_s *) refer_alloz(sizeof(miko_strlist_s))))
	{
		refer_hook_free(r, strlist);
		if (exbuffer_init(&r->data, 0) && hashmap_init(&r->strlist))
			return r;
		refer_free(r);
	}
	return NULL;
}

miko_strlist_s* miko_strlist_push(miko_strlist_s *restrict r, const char *restrict name, uintptr_t length)
{
	uintptr_t data_used;
	data_used = r->data.used;
	if (!exbuffer_append(&r->data, name?name:"", length + 1))
		goto label_fail;
	if (name)
	{
		if (hashmap_find_name(&r->strlist, name))
			goto label_fail;
		if (!hashmap_set_name(&r->strlist, name,
			(const void *) r->count, NULL))
			goto label_fail;
	}
	r->count += 1;
	return r;
	label_fail:
	r->data.used = data_used;
	return NULL;
}

miko_index_t miko_strlist_index(miko_strlist_s_t r, const char *restrict name)
{
	return name?((miko_index_t) hashmap_get_name(&r->strlist, name)):0;
}

const uint8_t* miko_strlist_data(miko_strlist_s_t r, uintptr_t *restrict rsize)
{
	if (rsize) *rsize = r->data.used;
	return r->data.data;
}
