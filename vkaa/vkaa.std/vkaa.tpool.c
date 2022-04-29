#include "std.h"
#include "std.tpool.h"

static vkaa_tpool_s* vkaa_std_create_tpool_do(vkaa_tpool_s *restrict tpool, const vkaa_std_typeid_t *typeid, uintptr_t *id, vkaa_type_s* (*func)(const vkaa_tpool_s *restrict tpool, const vkaa_std_typeid_t *restrict typeid))
{
	vkaa_tpool_s *rr;
	vkaa_type_s *restrict r;
	rr = NULL;
	*id = vkaa_tpool_genid(tpool);
	if ((r = func(tpool, typeid)))
	{
		rr = vkaa_tpool_insert(tpool, r);
		refer_free(r);
	}
	return rr;
}

vkaa_tpool_s* vkaa_std_create_tpool(vkaa_std_typeid_t *restrict typeid)
{
	vkaa_tpool_s *restrict r;
	if ((r = vkaa_tpool_alloc()))
	{
		#define d_type(_name)  vkaa_std_create_tpool_do(r, typeid, &typeid->id_##_name, vkaa_std_type_create_##_name)
		if (
			d_type(void)
		) return r;
		#undef d_type
		refer_free(r);
	}
	return NULL;
}
