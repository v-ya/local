#include "miko.wink.h"
#include "miko.wink.gomi.h"
#include "miko.wink.see.h"
#include "miko.list.api.h"
#include <memory.h>
#include <stdlib.h>

void miko_wink_w_free(miko_wink_w *restrict r)
{
	if (r->free) r->free(r + 1);
	refer_ck_free(r->rlock);
	refer_ck_free(r->wlock);
	free(r);
}

static inline miko_wink_w* miko_wink_initial(miko_wink_w *restrict r, miko_wink_gomi_s *restrict gomi, uintptr_t size, miko_wink_init_f init, miko_wink_view_f view, miko_wink_free_f free)
{
	r->inode.next = NULL;
	r->gomi = gomi;
	r->rlock = NULL;
	r->wlock = NULL;
	r->view = view;
	r->free = free;
	r->refs = 1;
	r->visible = miko_wink_visible__outside;
	if (init) init(r + 1);
	if (!yaw_lock_alloc_rwlock(&r->rlock, &r->wlock))
	{
		miko_list_insert(&gomi->root, &r->inode);
		return r;
	}
	return NULL;
}

miko_wink_t miko_wink_alloc(miko_wink_gomi_s *restrict gomi, uintptr_t size, miko_wink_init_f init, miko_wink_view_f view, miko_wink_free_f free)
{
	miko_wink_w *restrict r;
	if ((r = (miko_wink_w *) malloc(size + sizeof(miko_wink_w))))
	{
		if (miko_wink_initial(r, gomi, size, init, view, free))
			return (miko_wink_t) (r + 1);
		miko_wink_w_free(r);
	}
	return NULL;
}

miko_wink_t miko_wink_alloz(miko_wink_gomi_s *restrict gomi, uintptr_t size, miko_wink_init_f init, miko_wink_view_f view, miko_wink_free_f free)
{
	miko_wink_w *restrict r;
	if ((r = (miko_wink_w *) calloc(1, size + sizeof(miko_wink_w))))
	{
		if (miko_wink_initial(r, gomi, size, init, view, free))
			return (miko_wink_t) (r + 1);
		miko_wink_w_free(r);
	}
	return NULL;
}

#define miko_wink_fetch(_wink_)  ((miko_wink_w *) (_wink_) - 1)

miko_wink_t miko_wink_ref(miko_wink_t wink)
{
	volatile uintptr_t *restrict refs;
	uintptr_t n;
	if (wink)
	{
		refs = &miko_wink_fetch(wink)->refs;
		while ((n = *refs))
		{
			if (__sync_bool_compare_and_swap(refs, n, n + 1))
				return wink;
		}
	}
	return NULL;
}

miko_wink_t miko_wink_unref(miko_wink_t wink)
{
	volatile uintptr_t *restrict refs;
	uintptr_t n;
	refs = &miko_wink_fetch(wink)->refs;
	while ((n = *refs))
	{
		if (__sync_bool_compare_and_swap(refs, n, n - 1))
			return (n > 1)?wink:NULL;
	}
	return NULL;
}

miko_wink_t miko_wink_used(miko_wink_t wink)
{
	miko_wink_w *restrict r;
	if (wink)
	{
		r = miko_wink_fetch(wink);
		miko_wink_see_wink_to_gomi(r->gomi, r);
	}
	return wink;
}

void miko_wink_rlock_lock(miko_wink_t wink)
{
	yaw_lock_s *restrict lock;
	lock = miko_wink_fetch(wink)->rlock;
	yaw_lock_lock(lock);
}

void miko_wink_rlock_unlock(miko_wink_t wink)
{
	yaw_lock_s *restrict lock;
	lock = miko_wink_fetch(wink)->rlock;
	yaw_lock_unlock(lock);
}

void miko_wink_wlock_lock(miko_wink_t wink)
{
	yaw_lock_s *restrict lock;
	lock = miko_wink_fetch(wink)->wlock;
	yaw_lock_lock(lock);
}

void miko_wink_wlock_unlock(miko_wink_t wink)
{
	yaw_lock_s *restrict lock;
	lock = miko_wink_fetch(wink)->wlock;
	yaw_lock_unlock(lock);
}
