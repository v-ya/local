#include "miko.wink.link.h"
#include "miko.wink.see.h"

static void miko_wink_link_free_func(miko_wink_link_s *restrict r)
{
	refer_ck_free(r->read);
	refer_ck_free(r->write);
	rbtree_clear(&r->wink);
}

miko_wink_link_s* miko_wink_link_alloc(miko_wink_gomi_s *restrict gomi)
{
	miko_wink_link_s *restrict r;
	if ((r = (miko_wink_link_s *) refer_alloz(sizeof(miko_wink_link_s))))
	{
		refer_hook_free(r, link);
		r->gomi = gomi;
		if (!yaw_lock_alloc_rwlock(&r->read, &r->write))
			return r;
		refer_free(r);
	}
	return NULL;
}

rbtree_t* miko_wink_link_insert(miko_wink_link_s *restrict r, miko_wink_s *restrict item)
{
	yaw_lock_s *restrict lock;
	rbtree_t *restrict rbv;
	rbtree_t *restrict rbv_insert;
	lock = r->write;
	rbv_insert = NULL;
	yaw_lock_lock(lock);
	if ((rbv = rbtree_find(&r->wink, NULL, (uintptr_t) item)) ||
		(rbv = rbv_insert = rbtree_insert(&r->wink, NULL, (uintptr_t) item, NULL, NULL)))
		rbv->value = (void *) ((uintptr_t) rbv->value + 1);
	yaw_lock_unlock(lock);
	// notify gomi re-search item
	if (rbv_insert)
		miko_wink_see_wink_to_gomi(r->gomi, item);
	return rbv;
}

void miko_wink_link_delete(miko_wink_link_s *restrict r, miko_wink_s *restrict item)
{
	yaw_lock_s *restrict lock;
	rbtree_t *restrict rbv;
	lock = r->write;
	yaw_lock_lock(lock);
	if ((rbv = rbtree_find(&r->wink, NULL, (uintptr_t) item)) &&
		!(rbv->value = (void *) ((uintptr_t) rbv->value - 1)))
		rbtree_delete_by_pointer(&r->wink, rbv);
	yaw_lock_unlock(lock);
}

void miko_wink_link_call(miko_wink_link_s *restrict r, rbtree_func_call_f call_func, void *call_data)
{
	yaw_lock_s *restrict lock;
	lock = r->read;
	yaw_lock_lock(lock);
	if (r->wink) rbtree_call(&r->wink, call_func, call_data);
	yaw_lock_unlock(lock);
}
