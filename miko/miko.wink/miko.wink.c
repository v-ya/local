#include "miko.wink.h"
#include "miko.wink.link.h"
#include "miko.wink.gomi.h"
#include "miko.list.api.h"
#include <memory.h>

static void miko_wink_free_func(miko_wink_s *restrict r)
{
	if (r->free) r->free(r + 1);
	refer_ck_free(r->link);
}

static miko_wink_s* miko_wink_initial(miko_wink_s *restrict r, miko_wink_gomi_s *restrict gomi)
{
	refer_set_free(r, (refer_free_f) miko_wink_free_func);
	if ((r->link = miko_wink_link_alloc(gomi)))
	{
		r->status = miko_wink_status__root;
		r->visible = miko_wink_visible__visit;
		miko_list_insert(&gomi->root, &r->inode);
		return r;
	}
	return NULL;
}

miko_wink_t miko_wink_alloc(miko_wink_gomi_s *restrict gomi, uintptr_t size)
{
	miko_wink_s *restrict r;
	if ((r = (miko_wink_s *) refer_alloc(size + sizeof(miko_wink_s))))
	{
		memset(r, 0, sizeof(miko_wink_s));
		if (miko_wink_initial(r, gomi))
			return (miko_wink_t) (r + 1);
		refer_free(r);
	}
	return NULL;
}

miko_wink_t miko_wink_alloz(miko_wink_gomi_s *restrict gomi, uintptr_t size)
{
	miko_wink_s *restrict r;
	if ((r = (miko_wink_s *) refer_alloz(size + sizeof(miko_wink_s))))
	{
		if (miko_wink_initial(r, gomi))
			return (miko_wink_t) (r + 1);
		refer_free(r);
	}
	return NULL;
}

#define miko_wink_fetch(_wink_)  ((miko_wink_s *) (_wink_) - 1)

void miko_wink_set_free(miko_wink_t wink, refer_free_f free_func)
{
	miko_wink_fetch(wink)->free = free_func;
}

miko_wink_t miko_wink_link(miko_wink_t parent, miko_wink_t child)
{
	if (child && miko_wink_link_insert(miko_wink_fetch(parent)->link, miko_wink_fetch(child)))
		return child;
	return NULL;
}

void miko_wink_unlink(miko_wink_t parent, miko_wink_t child)
{
	if (child) miko_wink_link_delete(miko_wink_fetch(parent)->link, miko_wink_fetch(child));
}

void miko_wink_lost(miko_wink_t wink)
{
	__sync_val_compare_and_swap(&miko_wink_fetch(wink)->status, miko_wink_status__root, miko_wink_status__lost);
}
