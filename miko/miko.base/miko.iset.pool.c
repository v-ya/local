#include "miko.iset.pool.h"
#include "miko.iset.h"
#include <vattr.h>

const miko_iset_pool_s* miko_iset_pool_save_depend(const miko_iset_pool_s *restrict pool, const miko_iset_s *restrict iset)
{
	const vattr_vlist_t *restrict vlist;
	const miko_iset_s *restrict depend;
	vattr_s *restrict result;
	refer_string_t name;
	if ((result = vattr_alloc()))
	{
		if (!vattr_insert_tail(result, iset->name, iset))
			goto label_fail;
		for (vlist = iset->depend->vattr; vlist; vlist = vlist->vattr_next)
		{
			name = (refer_string_t) vlist->value;
			if (!(depend = (const miko_iset_s *) vattr_get_first(pool, name)) ||
				!vattr_insert_tail(result, depend->name, depend))
				goto label_fail;
		}
		return result;
		label_fail:
		refer_free(result);
	}
	return NULL;
}

refer_string_t miko_iset_pool_save_type(const miko_iset_pool_s *restrict pool, const char *restrict name)
{
	const vattr_vlist_t *restrict vlist;
	const miko_iset_s *restrict iset;
	refer_string_t result;
	for (vlist = pool->vattr; vlist; vlist = vlist->vattr_next)
	{
		iset = (const miko_iset_s *) vlist->value;
		if ((result = (refer_string_t) vattr_get_first(iset->type, name)))
			return refer_save(result);
	}
	return NULL;
}
