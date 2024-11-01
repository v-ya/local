#include "miko.wink.search.h"

static void miko_wink_searched_free_func(miko_wink_searched_s *restrict r)
{
	rbtree_clear(&r->exist);
}

miko_wink_searched_s* miko_wink_searched_alloc(void)
{
	miko_wink_searched_s *restrict r;
	if ((r = (miko_wink_searched_s *) refer_alloz(sizeof(miko_wink_searched_s))))
	{
		refer_hook_free(r, searched);
		return r;
	}
	return NULL;
}

void miko_wink_searched_clear(miko_wink_searched_s *restrict r)
{
	rbtree_clear(&r->exist);
}

static void miko_wink_search_free_func(miko_wink_search_s *restrict r)
{
	exbuffer_uini(&r->wink_array);
	refer_ck_free(r->searched);
}

miko_wink_search_s* miko_wink_search_alloc(miko_wink_searched_s *restrict searched)
{
	miko_wink_search_s *restrict r;
	if ((r = (miko_wink_search_s *) refer_alloz(sizeof(miko_wink_search_s))))
	{
		refer_hook_free(r, search);
		if (exbuffer_init(&r->wink_array, 0) &&
			((r->searched = (miko_wink_searched_s *) refer_save(searched))))
			return r;
		refer_free(r);
	}
	return NULL;
}

miko_wink_search_s* miko_wink_search_append(miko_wink_search_s *restrict r, miko_wink_w *restrict wink)
{
	rbtree_t *restrict *restrict pp;
	rbtree_t *restrict rbv;
	if (!wink || rbtree_find(pp = &r->searched->exist, NULL, (uintptr_t) wink))
		goto label_okay;
	if ((rbv = rbtree_insert(pp, NULL, (uintptr_t) wink, NULL, NULL)))
	{
		if (exbuffer_append(&r->wink_array, (const void *) &wink, sizeof(wink)))
		{
			label_okay:
			return r;
		}
		rbtree_delete_by_pointer(pp, rbv);
	}
	return NULL;
}

miko_wink_w** miko_wink_search_data(const miko_wink_search_s *restrict r, uintptr_t *restrict count)
{
	if (count) *count = r->wink_array.used / sizeof(miko_wink_w *);
	return (miko_wink_w **) r->wink_array.data;
}

void miko_wink_search_clear(miko_wink_search_s *restrict r)
{
	exbuffer_clear(&r->wink_array);
}
