#include "std.h"
#include "std.tpool.h"

static void vkaa_std_create_tpool_initial_id(vkaa_std_typeid_t *restrict typeid, vkaa_tpool_s *restrict tpool)
{
	typeid->id_void = vkaa_tpool_genid(tpool);
}

static vkaa_tpool_s* vkaa_std_create_tpool_do(vkaa_tpool_s *restrict tpool, const vkaa_std_typeid_t *restrict typeid, vkaa_type_s* (*func)(const vkaa_tpool_s *restrict tpool, const vkaa_std_typeid_t *restrict typeid))
{
	vkaa_tpool_s *rr;
	vkaa_type_s *restrict r;
	rr = NULL;
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
		vkaa_std_create_tpool_initial_id(typeid, r);
		#define d_type(_name)  vkaa_std_create_tpool_do(r, typeid, vkaa_std_type_create_##_name)
		if (
			d_type(void) &&
			vkaa_tpool_var_const_enable_by_id(r, typeid->id_void)
		) return r;
		#undef d_type
		refer_free(r);
	}
	return NULL;
}
