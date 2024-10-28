#include "miko.wink.link.h"

static void miko_wink_link_free_func(miko_wink_link_s *restrict r)
{
	refer_ck_free(r->read);
	refer_ck_free(r->write);
	rbtree_clear(&r->wink);
}

miko_wink_link_s* miko_wink_link_alloc(const miko_wink_batch_t *restrict batch)
{
	miko_wink_link_s *restrict r;
	if ((r = (miko_wink_link_s *) refer_alloz(sizeof(miko_wink_link_s))))
	{
		refer_hook_free(r, link);
		r->batch = batch;
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
	uintptr_t modify;
	lock = r->write;
	yaw_lock_lock(lock);
	if ((rbv = rbtree_find(&r->wink, NULL, (uintptr_t) item)) ||
		(rbv = rbtree_insert(&r->wink, NULL, (uintptr_t) item, NULL, NULL)))
		rbv->value = (void *) ((uintptr_t) rbv->value + 1);
	yaw_lock_unlock(lock);
	modify = miko_wink_batch_modify(*r->batch);
	item->link->modify = modify;
	r->modify = modify;
	return rbv;
}

void miko_wink_link_delete(miko_wink_link_s *restrict r, miko_wink_s *restrict item)
{
	yaw_lock_s *restrict lock;
	rbtree_t *restrict rbv;
	uintptr_t modify;
	modify = miko_wink_batch_modify(*r->batch);
	item->link->modify = modify;
	r->modify = modify;
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
	rbtree_call(&r->wink, call_func, call_data);
	yaw_lock_unlock(lock);
}
