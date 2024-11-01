#include "miko.table.h"

static void miko_table_impl_free_func(miko_table_impl_s *restrict r)
{
	refer_ck_free(r->range);
	refer_ck_free(r->vector);
}

static miko_table_impl_s* miko_table_alloc(void)
{
	miko_table_impl_s *restrict r;
	refer_t refer;
	if ((r = ((miko_table_impl_s *) refer_alloz(sizeof(miko_table_impl_s)))))
	{
		refer_hook_free(r, table_impl);
		refer = NULL;
		if ((r->vector = miko_vector_alloc(sizeof(refer_t),
			(miko_vector_initial_f) refer_save,
			(miko_vector_finally_f) (miko_func_f) refer_free)) &&
			(r->range = vattr_alloc()) &&
			miko_vector_push(r->vector, &refer, sizeof(refer_t)))
			return r;
		refer_free(r);
	}
	return NULL;
}

static miko_table_impl_s* miko_table_set_range(miko_table_impl_s *restrict r, const char *restrict name, miko_index_t index, miko_count_t count)
{
	miko_table_range_s *restrict range;
	if ((range = ((miko_table_range_s *) refer_alloz(sizeof(miko_table_range_s)))))
	{
		range->index = index;
		range->count = count;
		if (!vattr_set(r->range, name, range))
			r = NULL;
		refer_free(range);
		return r;
	}
	return NULL;
}

static miko_table_impl_s* miko_table_initial_single(miko_table_impl_s *restrict r, vattr_s *restrict pool)
{
	vattr_vlist_t *restrict vlist;
	uintptr_t index;
	for (vlist = pool->vattr; vlist; vlist = vlist->vattr_next)
	{
		if (vlist->vslot->vslot == vlist)
		{
			index = miko_vector_count(r->vector);
			if (!miko_vector_push(r->vector, &vlist->value, sizeof(refer_t)))
				goto label_fail;
			if (!miko_table_set_range(r, vlist->vslot->key, index, 1))
				goto label_fail;
		}
	}
	return r;
	label_fail:
	return NULL;
}

static miko_table_impl_s* miko_table_initial_multi(miko_table_impl_s *restrict r, vattr_s *restrict pool)
{
	vattr_vlist_t *restrict vlist;
	vattr_vlist_t *restrict vslot;
	uintptr_t index, count;
	for (vlist = pool->vattr; vlist; vlist = vlist->vattr_next)
	{
		if (vlist->vslot->vslot == vlist)
		{
			index = miko_vector_count(r->vector);
			count = 0;
			for (vslot = vlist; vslot; vslot = vslot->vslot_next)
			{
				if (!miko_vector_push(r->vector, &vslot->value, sizeof(refer_t)))
					goto label_fail;
				count += 1;
			}
			if (!miko_table_set_range(r, vlist->vslot->key, index, count))
				goto label_fail;
		}
	}
	return r;
	label_fail:
	return NULL;
}

miko_table_s* miko_table_create_single(struct vattr_s *restrict pool)
{
	miko_table_impl_s *restrict r;
	if (pool && (r = miko_table_alloc()))
	{
		if (miko_table_initial_single(r, pool))
			return &r->table;
		refer_free(r);
	}
	return NULL;
}

miko_table_s* miko_table_create_multi(struct vattr_s *restrict pool)
{
	miko_table_impl_s *restrict r;
	if (pool && (r = miko_table_alloc()))
	{
		if (miko_table_initial_multi(r, pool))
			return &r->table;
		refer_free(r);
	}
	return NULL;
}

#define _impl_(_table_)  ((miko_table_impl_s *) (_table_))

miko_index_t miko_table_index(const miko_table_s *restrict r, const char *restrict name)
{
	const miko_table_range_s *restrict range;
	if (name && (range = (const miko_table_range_s *) vattr_get_first(_impl_(r)->range, name)))
		return range->index;
	return 0;
}

miko_count_t miko_table_count(const miko_table_s *restrict r, const char *restrict name)
{
	const miko_table_range_s *restrict range;
	if (name && (range = (const miko_table_range_s *) vattr_get_first(_impl_(r)->range, name)))
		return range->count;
	return 0;
}
