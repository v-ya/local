#include "statistics.h"
#include <mlog.h>
#include <stdlib.h>

static void statistics_item_free_func(statistics_item_s *restrict r)
{
	if (r->uri) refer_free(r->uri);
}

static void statistics_free_func(statistics_s *restrict r)
{
	if (r->st) refer_free(r->st);
	if (r->r) refer_free(r->r);
	if (r->w) refer_free(r->w);
}

static statistics_item_s* statistics_item_alloc(refer_nstring_t uri)
{
	statistics_item_s *restrict r;
	if ((r = (statistics_item_s *) refer_alloc(sizeof(statistics_item_s))))
	{
		r->uri = refer_save(uri);
		r->count = 0;
		refer_set_free(r, (refer_free_f) statistics_item_free_func);
	}
	return r;
}

static inline void statistics_item_inc(statistics_item_s *restrict r)
{
	__sync_fetch_and_add(&r->count, 1);
}

statistics_s* statistics_alloc(void)
{
	statistics_s *restrict r;
	if ((r = (statistics_s *) refer_alloz(sizeof(statistics_s))))
	{
		refer_set_free(r, (refer_free_f) statistics_free_func);
		if ((r->st = vattr_alloc()) &&
			!yaw_lock_alloc_rwlock(&r->r, &r->w))
			return r;
		refer_free(r);
	}
	return NULL;
}

statistics_s* statistics_touch(statistics_s *restrict r, refer_nstring_t uri)
{
	statistics_item_s *restrict item;
	yaw_lock_lock(r->r);
	item = (statistics_item_s *) refer_save(vattr_get_first(r->st, uri->string));
	yaw_lock_unlock(r->r);
	if (item)
	{
		label_inc_and_free:
		statistics_item_inc(item);
		refer_free(item);
		return r;
	}
	else
	{
		yaw_lock_lock(r->w);
		if (!(item = (statistics_item_s *) refer_save(vattr_get_first(r->st, uri->string))))
		{
			if ((item = statistics_item_alloc(uri)) &&
				!vattr_insert_tail(r->st, uri->string, item))
			{
				refer_free(item);
				item = NULL;
			}
		}
		yaw_lock_unlock(r->w);
		if (item) goto label_inc_and_free;
	}
	return NULL;
}

// get

typedef struct statistics_getdaze_t statistics_getdaze_t;
struct statistics_getdaze_t {
	statistics_getdaze_t *next;
	refer_nstring_t uri;
	uintptr_t count;
};

static void statistics_rbt_free_func(rbtree_t *restrict inode)
{
	statistics_getdaze_t *restrict v, *n;
	if ((n = (statistics_getdaze_t *) inode->value))
	{
		while ((v = n))
		{
			n = v->next;
			if (v->uri)
				refer_free(v->uri);
			free(v);
		}
	}
}

static void statistics_rbt_push(rbtree_t *restrict *restrict rbt, statistics_item_s *restrict item)
{
	rbtree_t *restrict inode;
	statistics_getdaze_t *restrict v;
	uintptr_t count;
	uint64_t key;
	count = item->count;
	key = ~(uint64_t) count;
	if ((inode = rbtree_find(rbt, NULL, key)) ||
		(inode = rbtree_insert(rbt, NULL, key, NULL, statistics_rbt_free_func)))
	{
		if ((v = (statistics_getdaze_t *) malloc(sizeof(statistics_getdaze_t))))
		{
			v->uri = refer_save(item->uri);
			v->count = count;
			v->next = inode->value;
			inode->value = v;
		}
	}
}

static void statistics_dump_func(rbtree_t *restrict inode, mlog_s *ml)
{
	statistics_getdaze_t *restrict v;
	if ((v = (statistics_getdaze_t *) inode->value))
	{
		do {
			mlog_printf(ml, "[%8zu] %s\n", v->count, v->uri->string);
		} while ((v = v->next));
	}
}

void statistics_dump(statistics_s *restrict r)
{
	extern mlog_s *$mlog;
	rbtree_t *restrict rbt;
	vattr_vlist_t *restrict vl;
	statistics_item_s *restrict item;
	rbt = NULL;
	yaw_lock_lock(r->r);
	for (vl = r->st->vattr; vl; vl = vl->vattr_next)
	{
		if ((item = (statistics_item_s *) vl->value) && item->uri)
			statistics_rbt_push(&rbt, item);
	}
	yaw_lock_unlock(r->r);
	rbtree_call(&rbt, (rbtree_func_call_f) statistics_dump_func, $mlog);
	rbtree_clear(&rbt);
}
