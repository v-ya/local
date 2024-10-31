#include "miko.iset.pool.h"
#include "miko.iset.h"
#include "miko.label.fail.h"
#include <vattr.h>

const miko_iset_pool_s* miko_iset_pool_save_depend(const miko_iset_pool_s *restrict pool, const miko_iset_s *restrict iset, miko_log_s *restrict log)
{
	const vattr_vlist_t *restrict vlist;
	const miko_iset_s *restrict depend;
	vattr_s *restrict result;
	refer_string_t name;
	if (!(result = vattr_alloc()))
		goto label_fail_memless;
	if (!vattr_insert_tail(result, iset->name, iset))
		goto label_fail_memless;
	for (vlist = iset->depend->vattr; vlist; vlist = vlist->vattr_next)
	{
		name = (refer_string_t) vlist->value;
		if (!(depend = (const miko_iset_s *) vattr_get_first(pool, name)))
			goto label_fail_miss;
		if (!vattr_insert_tail(result, depend->name, depend))
			goto label_fail_memless;
	}
	return result;
	label_fail:
	refer_ck_free(result);
	return NULL;
	miko_label_fail(memless, miko_log_add_key, log, iset->name);
	miko_label_fail(miss, miko_log_add_key, log, name);
}

#define miko_iset_pool_save(_type_, _field_)  \
	{\
		const vattr_vlist_t *restrict vlist;\
		const miko_iset_s *restrict iset;\
		_type_ result;\
		for (vlist = pool->vattr; vlist; vlist = vlist->vattr_next)\
		{\
			iset = (const miko_iset_s *) vlist->value;\
			if ((result = (_type_) vattr_get_first(iset->_field_, name)))\
				return (_type_) refer_save(result);\
		}\
		return NULL;\
	}

refer_string_t miko_iset_pool_save_segment(const miko_iset_pool_s *restrict pool, const char *restrict name)
	miko_iset_pool_save(refer_string_t, segment)

refer_string_t miko_iset_pool_save_action(const miko_iset_pool_s *restrict pool, const char *restrict name)
	miko_iset_pool_save(refer_string_t, action)

miko_major_s* miko_iset_pool_save_major(const miko_iset_pool_s *restrict pool, const char *restrict name)
	miko_iset_pool_save(miko_major_s *, major)

#undef miko_iset_pool_save
