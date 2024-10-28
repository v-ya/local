#include "miko.wink.gomi.h"
#include "miko.wink.link.h"
#include "miko.wink.search.h"
#include "miko.wink.see.h"
#include "miko.list.api.h"

// inline function

static inline void miko_wink_visible(miko_wink_s *restrict wink, miko_wink_visible_t visible)
{
	if (wink->visible < visible)
		wink->visible = visible;
}

// gomi visible layer

static void miko_wink_gomi_visible_layer_go_func(rbtree_t *restrict rbv, miko_wink_search_s **restrict result)
{
	miko_wink_s *restrict wink;
	if (*result)
	{
		wink = (miko_wink_s *) (uintptr_t) rbv->key_index;
		if (wink->visible == miko_wink_visible__lost)
			*result = miko_wink_search_append(*result, wink);
	}
}

static miko_wink_search_s* miko_wink_gomi_visible_layer_once(miko_wink_search_s *restrict search, miko_wink_s **restrict wink_array, uintptr_t wink_count)
{
	miko_wink_s *restrict wink;
	miko_wink_search_s *volatile result;
	uintptr_t index;
	result = search;
	miko_wink_search_clear(search);
	for (index = 0; result && index < wink_count; ++index)
	{
		wink = wink_array[index];
		miko_wink_visible(wink, miko_wink_visible__wink);
		miko_wink_link_call(wink->link, (rbtree_func_call_f) miko_wink_gomi_visible_layer_go_func, (void *) &result);
	}
	return result;
}

static miko_wink_gomi_s* miko_wink_gomi_visible_layer_collect(miko_wink_gomi_s *restrict gomi, miko_wink_search_s *restrict search)
{
	miko_list_t **restrict p;
	miko_wink_see_s *restrict see;
	p = &gomi->linked;
	while ((see = (miko_wink_see_s *) *p))
	{
		if (!miko_wink_search_append(search, see->wink))
			goto label_fail;
		miko_list_unlink(p, &see->inode);
		refer_free(see);
	}
	return gomi;
	label_fail:
	return NULL;
}

miko_wink_gomi_s* miko_wink_gomi_visible_layer(miko_wink_gomi_s *restrict gomi, miko_wink_search_s *restrict search, miko_wink_search_s *restrict cache, miko_wink_report_t *restrict report)
{
	miko_wink_search_s *restrict swap;
	miko_wink_s **restrict wink_array;
	uintptr_t wink_count;
	if (!miko_wink_gomi_visible_layer_collect(gomi, cache))
		goto label_fail;
	while ((wink_array = miko_wink_search_data(search, &wink_count)) && wink_count)
	{
		if (!miko_wink_gomi_visible_layer_once(cache, wink_array, wink_count))
			goto label_fail;
		if (!miko_wink_gomi_visible_layer_collect(gomi, cache))
			goto label_fail;
		report->layer_done_count += 1;
		swap = search;
		search = cache;
		cache = swap;
	}
	return gomi;
	label_fail:
	return NULL;
}

#define miko_wink_from_inode(_inode_)  ((miko_wink_s *) (_inode_))
#define miko_wink_to_inode(_wink_)     (&(_wink_)->inode)

miko_wink_gomi_s* miko_wink_gomi_visible_initial(miko_wink_gomi_s *restrict gomi, miko_wink_search_s *restrict search, miko_wink_report_t *restrict report)
{
	miko_wink_s *restrict wink;
	miko_wink_search_clear(search);
	for (wink = miko_wink_from_inode(gomi->root); wink; wink = miko_wink_from_inode(wink->inode.next))
	{
		if (wink->status == miko_wink_status__lost)
		{
			wink->visible = miko_wink_visible__lost;
			report->former_lost_inode += 1;
		}
		else
		{
			wink->visible = miko_wink_visible__visit;
			report->former_root_inode += 1;
			if (!miko_wink_search_append(search, wink))
				gomi = NULL;
		}
	}
	return gomi;
}

void miko_wink_gomi_visible_finally(miko_wink_gomi_s *restrict gomi, miko_wink_report_t *restrict report)
{
	miko_list_t **restrict p;
	miko_wink_s *restrict wink;
	uint32_t status, visible;
	p = &gomi->root;
	while ((wink = miko_wink_from_inode(*p)))
	{
		status = wink->status;
		visible = wink->visible;
		if (status == miko_wink_status__lost)
			report->latter_lost_inode += 1;
		else report->latter_root_inode += 1;
		switch (visible)
		{
			case miko_wink_visible__lost: report->visible_lost_inode += 1; break;
			case miko_wink_visible__wink: report->visible_wink_inode += 1; break;
			case miko_wink_visible__visit: report->visible_visit_inode += 1; break;
			default: break;
		}
		if (visible != miko_wink_visible__lost)
			p = &wink->inode.next;
		else
		{
			miko_list_unlink(p, miko_wink_to_inode(wink));
			refer_free(wink);
		}
	}
}
