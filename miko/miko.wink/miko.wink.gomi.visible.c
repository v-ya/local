#include "miko.wink.gomi.h"
#include "miko.wink.search.h"
#include "miko.wink.see.h"
#include "miko.list.api.h"

// gomi visible layer

static miko_wink_search_s* miko_wink_gomi_visible_layer_once(miko_wink_search_s *restrict search, miko_wink_w **restrict wink_array, uintptr_t wink_count)
{
	miko_wink_w *restrict wink;
	yaw_lock_s *restrict lock;
	miko_wink_view_f view;
	uintptr_t index;
	miko_wink_search_clear(search);
	for (index = 0; search && index < wink_count; ++index)
	{
		wink = wink_array[index];
		wink->visible = miko_wink_visible__inside;
		if ((view = wink->view))
		{
			lock = wink->rlock;
			yaw_lock_lock(lock);
			search = view(search, wink + 1);
			yaw_lock_unlock(lock);
		}
	}
	return search;
}

static miko_wink_gomi_s* miko_wink_gomi_visible_layer_collect(miko_wink_gomi_s *restrict gomi, miko_wink_search_s *restrict search)
{
	miko_list_t **restrict p;
	miko_wink_see_t *restrict see;
	p = &gomi->linked;
	while ((see = (miko_wink_see_t *) *p))
	{
		if (!miko_wink_search_append(search, see->wink))
			goto label_fail;
		miko_list_unlink(p, &see->inode);
		miko_wink_see_free(see);
	}
	return gomi;
	label_fail:
	return NULL;
}

miko_wink_gomi_s* miko_wink_gomi_visible_layer(miko_wink_gomi_s *restrict gomi, miko_wink_search_s *restrict search, miko_wink_search_s *restrict cache, miko_wink_report_t *restrict report)
{
	miko_wink_search_s *restrict swap;
	miko_wink_w **restrict wink_array;
	uintptr_t wink_count;
	if (!miko_wink_gomi_visible_layer_collect(gomi, search))
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

miko_wink_gomi_s* miko_wink_gomi_visible_initial(miko_wink_gomi_s *restrict gomi, miko_wink_search_s *restrict search, miko_wink_report_t *restrict report)
{
	miko_list_t **restrict p;
	miko_wink_w *restrict wink;
	miko_wink_search_clear(search);
	p = &gomi->root;
	while ((wink = (miko_wink_w *) *p))
	{
		if (!wink->refs)
		{
			miko_list_unlink(p, &wink->inode);
			miko_list_insert(&gomi->lost, &wink->inode);
		}
		else
		{
			wink->visible = miko_wink_visible__outside;
			if (!miko_wink_search_append(search, wink))
				gomi = NULL;
			p = &wink->inode.next;
			report->former_root_inode += 1;
		}
	}
	for (wink = (miko_wink_w *) gomi->lost; wink; wink = (miko_wink_w *) wink->inode.next)
	{
		wink->visible = miko_wink_visible__outside;
		report->former_lost_inode += 1;
	}
	return gomi;
}

void miko_wink_gomi_visible_finally(miko_wink_gomi_s *restrict gomi, miko_wink_report_t *restrict report)
{
	miko_list_t **restrict p;
	miko_wink_w *restrict wink;
	p = &gomi->lost;
	while ((wink = (miko_wink_w *) *p))
	{
		if (wink->visible != miko_wink_visible__outside)
		{
			p = &wink->inode.next;
			report->latter_lost_inode += 1;
		}
		else
		{
			miko_list_unlink(p, &wink->inode);
			miko_wink_w_free(wink);
			report->latter_free_inode += 1;
		}
	}
}
