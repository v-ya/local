#include "miko.table.h"

static void* miko_table_impl_vector_initial_func(refer_t *restrict r)
{
	refer_save(*r);
	return r;
}

static void miko_table_impl_vector_finally_func(refer_t *restrict r)
{
	refer_ck_free(*r);
}

static void miko_table_impl_free_func(miko_table_impl_s *restrict r)
{
	refer_ck_free(r->range);
	refer_ck_free(r->vector);
}

static miko_table_impl_s* miko_table_alloc(void)
{
	miko_table_impl_s *restrict r;
	if ((r = ((miko_table_impl_s *) refer_alloz(sizeof(miko_table_impl_s)))))
	{
		refer_hook_free(r, table_impl);
		if ((r->vector = miko_vector_alloc(sizeof(refer_t),
			(miko_vector_initial_f) miko_table_impl_vector_initial_func,
			(miko_vector_finally_f) miko_table_impl_vector_finally_func)) &&
			(r->range = vattr_alloc()) &&
			miko_vector_push_zero(r->vector, 1))
			return r;
		refer_free(r);
	}
	return NULL;
}

static const miko_table_s* miko_table_okay(miko_table_impl_s *restrict r)
{
	r->table.table = miko_vector_data(r->vector);
	r->table.count = miko_vector_count(r->vector);
	return &r->table;
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
			if (!miko_vector_push(r->vector, &vlist->value, 1))
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
				if (!miko_vector_push(r->vector, &vslot->value, 1))
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

static miko_table_impl_s* miko_table_initial_builder(miko_table_impl_s *restrict r, const miko_table_s *restrict source, miko_table_builder_f builder)
{
	const refer_t *restrict p;
	refer_t value;
	uintptr_t i, n;
	miko_bool_t isokay;
	p = source->table;
	n = source->count;
	isokay = 1;
	for (i = 0; isokay && i < n; ++i)
	{
		value = builder(p[i], &isokay);
		if (!miko_vector_push(r->vector, &value, 1))
			goto label_fail;
	}
	if (isokay)
		return r;
	label_fail:
	return NULL;
}

const miko_table_s* miko_table_create_single(struct vattr_s *restrict pool)
{
	miko_table_impl_s *restrict r;
	if (pool && (r = miko_table_alloc()))
	{
		if (miko_table_initial_single(r, pool))
			return miko_table_okay(r);
		refer_free(r);
	}
	return NULL;
}

const miko_table_s* miko_table_create_multi(struct vattr_s *restrict pool)
{
	miko_table_impl_s *restrict r;
	if (pool && (r = miko_table_alloc()))
	{
		if (miko_table_initial_multi(r, pool))
			return miko_table_okay(r);
		refer_free(r);
	}
	return NULL;
}

const miko_table_s* miko_table_create_builder(const miko_table_s *restrict source, miko_table_builder_f builder)
{
	miko_table_impl_s *restrict r;
	if (source && builder && (r = miko_table_alloc()))
	{
		if (miko_table_initial_builder(r, source, builder))
			return miko_table_okay(r);
		refer_free(r);
	}
	return NULL;
}

#define _impl_(_table_)  ((miko_table_impl_s *) (_table_))

const miko_table_s* miko_table_range(const miko_table_s *restrict r, const char *restrict name, miko_index_t *restrict index, miko_count_t *restrict count)
{
	const miko_table_range_s *restrict range;
	if (name && (range = (const miko_table_range_s *) vattr_get_first(_impl_(r)->range, name)))
	{
		if (index) *index = range->index;
		if (count) *count = range->count;
		return r;
	}
	return NULL;
}

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
