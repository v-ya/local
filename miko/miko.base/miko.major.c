#include "miko.major.h"

static void* miko_major_vector_initial_func(refer_t *restrict r)
{
	refer_save(*r);
	return r;
}

static void miko_major_vector_finally_func(refer_t *restrict r)
{
	refer_ck_free(*r);
}

static void miko_major_free_func(miko_major_s *restrict r)
{
	refer_ck_free(r->name);
	refer_ck_free(r->iset);
	refer_ck_free(r->read);
	refer_ck_free(r->write);
	refer_ck_free(r->pool);
	refer_ck_free(r->table);
}

miko_major_s* miko_major_alloc(const char *restrict name, refer_string_t iset, miko_major_vtype_t vtype)
{
	miko_major_s *restrict r;
	if (name && iset && (r = (miko_major_s *) refer_alloz(sizeof(miko_major_s))))
	{
		refer_hook_free(r, major);
		r->vtype = vtype;
		if ((r->name = refer_dump_string(name)) &&
			(r->iset = (refer_string_t) refer_save(iset)) &&
			!yaw_lock_alloc_rwlock(&r->read, &r->write) &&
			(r->pool = vattr_alloc()) &&
			(r->table = miko_vector_alloc(sizeof(miko_minor_s *),
				(miko_vector_initial_f) miko_major_vector_initial_func,
				(miko_vector_finally_f) miko_major_vector_finally_func)) &&
			miko_vector_push_zero(r->table, 1))
			return r;
		refer_free(r);
	}
	return NULL;
}

miko_major_s* miko_major_add_minor(miko_major_s *restrict r, miko_minor_s *restrict minor)
{
	yaw_lock_s *restrict lock;
	miko_major_s *result;
	result = NULL;
	if (minor)
	{
		lock = r->write;
		yaw_lock_lock(lock);
		if (!vattr_get_vslot(r->pool, minor->name) &&
			vattr_insert_tail(r->pool, minor->name, minor))
		{
			if (miko_vector_push(r->table, (const void *) &minor, 1))
			{
				minor->minor = miko_vector_count(r->table) - 1;
				result = r;
			}
			else vattr_delete(r->pool, minor->name);
		}
		yaw_lock_unlock(lock);
	}
	return result;
}

miko_minor_s* miko_major_save_minor_by_index(const miko_major_s *restrict r, miko_index_t index)
{
	miko_minor_s *restrict minor;
	yaw_lock_s *restrict lock;
	lock = r->read;
	yaw_lock_lock(lock);
	refer_save(minor = (miko_minor_s *) miko_vector_index(r->table, index));
	yaw_lock_unlock(lock);
	return minor;
}

miko_minor_s* miko_major_save_minor_by_name(const miko_major_s *restrict r, const char *restrict name)
{
	miko_minor_s *restrict minor;
	yaw_lock_s *restrict lock;
	if (name)
	{
		lock = r->read;
		yaw_lock_lock(lock);
		refer_save(minor = (miko_minor_s *) vattr_get_first(r->pool, name));
		yaw_lock_unlock(lock);
		return minor;
	}
	return NULL;
}
