#include "miko.wink.pri.h"
#include "miko.list.api.h"

// wink search

static void miko_wink_search_free_func(miko_wink_search_s *restrict r)
{
	exbuffer_uini(&r->wink_array);
	rbtree_clear(&r->exist);
}

static miko_wink_search_s* miko_wink_search_alloc(void)
{
	miko_wink_search_s *restrict r;
	if ((r = (miko_wink_search_s *) refer_alloz(sizeof(miko_wink_search_s))))
	{
		refer_hook_free(r, wink_search);
		if (exbuffer_init(&r->wink_array, 0))
			return r;
		refer_free(r);
	}
	return NULL;
}

static miko_wink_search_s* miko_wink_search_append(miko_wink_search_s *restrict r, miko_wink_s *restrict wink)
{
	rbtree_t *restrict rbv;
	if (!wink || rbtree_find(&r->exist, NULL, (uintptr_t) wink))
		goto label_okay;
	if ((rbv = rbtree_insert(&r->exist, NULL, (uintptr_t) wink, NULL, NULL)))
	{
		if (exbuffer_append(&r->wink_array, (const void *) &wink, sizeof(wink)))
		{
			label_okay:
			return r;
		}
		rbtree_delete_by_pointer(&r->exist, rbv);
	}
	return NULL;
}

static miko_wink_s** miko_wink_search_data(const miko_wink_search_s *restrict r, uintptr_t *restrict count)
{
	if (count) *count = r->wink_array.used / sizeof(miko_wink_s *);
	return (miko_wink_s **) r->wink_array.data;
}

static void miko_wink_search_clear(miko_wink_search_s *restrict r)
{
	exbuffer_clear(&r->wink_array);
	rbtree_clear(&r->exist);
}

// wink to

static void miko_wink_to_free_func(miko_wink_to_s *restrict r)
{
	refer_ck_free(r->read);
	refer_ck_free(r->write);
	rbtree_clear(&r->wink);
}

static miko_wink_to_s* miko_wink_to_alloc(void)
{
	miko_wink_to_s *restrict r;
	if ((r = (miko_wink_to_s *) refer_alloz(sizeof(miko_wink_to_s))))
	{
		refer_hook_free(r, wink_to);
		if (!yaw_lock_alloc_rwlock(&r->read, &r->write))
			return r;
		refer_free(r);
	}
	return NULL;
}

static rbtree_t* miko_wink_to_link(miko_wink_to_s *restrict r, const miko_wink_s *restrict item)
{
	yaw_lock_s *restrict lock;
	rbtree_t *restrict rbv;
	lock = r->write;
	yaw_lock_lock(lock);
	if ((rbv = rbtree_find(&r->wink, NULL, (uintptr_t) item)) ||
		(rbv = rbtree_insert(&r->wink, NULL, (uintptr_t) item, NULL, NULL)))
		rbv->value = (void *) ((uintptr_t) rbv->value + 1);
	item->wink->modify = 1;
	r->modify = 1;
	yaw_lock_unlock(lock);
	return rbv;
}

static void miko_wink_to_unlink(miko_wink_to_s *restrict r, const miko_wink_s *restrict item)
{
	yaw_lock_s *restrict lock;
	rbtree_t *restrict rbv;
	lock = r->write;
	yaw_lock_lock(lock);
	if ((rbv = rbtree_find(&r->wink, NULL, (uintptr_t) item)) &&
		!(rbv->value = (void *) ((uintptr_t) rbv->value - 1)))
		rbtree_delete_by_pointer(&r->wink, rbv);
	item->wink->modify = 1;
	r->modify = 1;
	yaw_lock_unlock(lock);
}

static void miko_wink_to_call(miko_wink_to_s *restrict r, rbtree_func_call_f call_func, void *data)
{
	yaw_lock_s *restrict lock;
	lock = r->read;
	yaw_lock_lock(lock);
	rbtree_call(&r->wink, call_func, data);
	yaw_lock_unlock(lock);
}

// wink gomi

static inline void miko_wink_visible(miko_wink_s *restrict wink, miko_wink_visible_t visible)
{
	if (wink->visible < visible)
		wink->visible = visible;
}

static void miko_wink_gomi_wink_to_func(rbtree_t *restrict rbv, miko_wink_search_s **restrict result)
{
	miko_wink_s *restrict wink;
	if (*result)
	{
		wink = (miko_wink_s *) (uintptr_t) rbv->key_index;
		if (wink->visible == miko_wink_visible__lost)
			*result = miko_wink_search_append(*result, wink);
	}
}

static miko_wink_search_s* miko_wink_gomi_visible_layer(miko_wink_search_s *restrict search, miko_wink_s **restrict wink_array, uintptr_t wink_count)
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
		miko_wink_to_call(wink->wink, (rbtree_func_call_f) miko_wink_gomi_wink_to_func, (void *) &result);
	}
	return result;
}

static miko_wink_search_s* miko_wink_gomi_visible_deal(miko_wink_search_s *restrict search, miko_wink_search_s *restrict cache)
{
	miko_wink_search_s *restrict swap;
	miko_wink_s **restrict wink_array;
	uintptr_t wink_count;
	while ((wink_array = miko_wink_search_data(search, &wink_count)) && wink_count)
	{
		if (!miko_wink_gomi_visible_layer(cache, wink_array, wink_count))
			goto label_fail;
		swap = search;
		search = cache;
		cache = swap;
	}
	return search;
	label_fail:
	return NULL;
}

#define miko_wink_from_inode(_inode_)  ((miko_wink_s *) (_inode_))
#define miko_wink_to_inode(_wink_)     (&(_wink_)->inode)

static miko_wink_gomi_s* miko_wink_gomi_visible_reset(miko_wink_gomi_s *restrict gomi, miko_wink_search_s *restrict search)
{
	miko_wink_s *restrict wink;
	miko_wink_search_clear(search);
	for (wink = miko_wink_from_inode(gomi->root); wink; wink = miko_wink_from_inode(wink->inode.next))
	{
		wink->wink->modify = 0;
		if (wink->status == miko_wink_status__lost)
			wink->visible = miko_wink_visible__lost;
		else
		{
			wink->visible = miko_wink_visible__visit;
			if (!miko_wink_search_append(search, wink))
				gomi = NULL;
		}
	}
	return gomi;
}

static void miko_wink_gomi_visible_clear(miko_wink_gomi_s *restrict gomi)
{
	miko_list_t **restrict p;
	miko_wink_s *restrict wink;
	p = &gomi->root;
	while ((wink = miko_wink_from_inode(*p)))
	{
		if (wink->visible != miko_wink_visible__lost || wink->wink->modify)
			p = &wink->inode.next;
		else
		{
			miko_list_unlink(p, miko_wink_to_inode(wink));
			refer_free(wink);
		}
	}
}

#undef miko_wink_from_inode
#undef miko_wink_to_inode

static void miko_wink_gomi_daemon(yaw_s *restrict yaw)
{
	miko_wink_gomi_s *restrict gomi;
	uint64_t ts_curr, ts_next;
	gomi = (miko_wink_gomi_s *) yaw->data;
	ts_curr = ts_next = 0;
	while (yaw->running)
	{
		if ((ts_curr = yaw_timestamp_msec()) >= ts_next)
		{
			if ((ts_next += gomi->miko_gomi_msec) < ts_curr)
				ts_next = ts_curr;
			if (miko_wink_gomi_visible_reset(gomi, gomi->search) &&
				miko_wink_gomi_visible_deal(gomi->search, gomi->cache))
				miko_wink_gomi_visible_clear(gomi);
		}
		if (ts_curr < ts_next)
			yaw_msleep(50);
	}
}

static void miko_wink_gomi_free_func(miko_wink_gomi_s *restrict r)
{
	miko_list_t *restrict inode;
	if (r->daemon)
	{
		yaw_stop_and_wait(r->daemon);
		refer_free(r->daemon);
	}
	while ((inode = r->root))
	{
		miko_list_unlink(&r->root, inode);
		refer_free(inode);
	}
	refer_ck_free(r->search);
	refer_ck_free(r->cache);
}

miko_wink_gomi_s* miko_wink_gomi_alloc(uintptr_t miko_gomi_msec)
{
	miko_wink_gomi_s *restrict r;
	if ((r = (miko_wink_gomi_s *) refer_alloz(sizeof(miko_wink_gomi_s))))
	{
		refer_hook_free(r, wink_gomi);
		r->miko_gomi_msec = miko_gomi_msec;
		if ((r->search = miko_wink_search_alloc()) && (r->cache = miko_wink_search_alloc()) &&
			(r->daemon = yaw_alloc_and_start(miko_wink_gomi_daemon, NULL, r)))
			return r;
		refer_free(r);
	}
	return NULL;
}

// wink

static void miko_wink_free_func(miko_wink_s *restrict r)
{
	if (r->free) r->free(r);
	refer_ck_free(r->wink);
}

miko_wink_s* miko_wink_alloc(miko_wink_gomi_s *restrict gomi, uintptr_t size, refer_free_f free)
{
	miko_wink_s *restrict r;
	if (size >= sizeof(miko_wink_s) && (r = (miko_wink_s *) refer_alloz(size)))
	{
		refer_hook_free(r, wink);
		if ((r->wink = miko_wink_to_alloc()))
		{
			r->free = free;
			r->status = miko_wink_status__root;
			r->visible = miko_wink_visible__visit;
			miko_list_insert(&gomi->root, &r->inode);
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

const miko_wink_s* miko_wink_link(miko_wink_s *restrict parent, const miko_wink_s *restrict child)
{
	if (child && miko_wink_to_link(parent->wink, child))
		return child;
	return NULL;
}

void miko_wink_unlink(miko_wink_s *restrict parent, const miko_wink_s *restrict child)
{
	if (child) miko_wink_to_unlink(parent->wink, child);
}

void miko_wink_lost(miko_wink_s *restrict r)
{
	__sync_val_compare_and_swap(&r->status, miko_wink_status__root, miko_wink_status__lost);
}
