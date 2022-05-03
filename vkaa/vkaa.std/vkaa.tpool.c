#include "std.h"
#include "std.tpool.h"

static void vkaa_std_create_tpool_initial_id(vkaa_std_typeid_t *restrict typeid, vkaa_tpool_s *restrict tpool)
{
	typeid->id_void = vkaa_tpool_genid(tpool);
	typeid->id_scope = vkaa_tpool_genid(tpool);
}

vkaa_type_s* vkaa_std_tpool_set(vkaa_tpool_s *restrict tpool, const char *restrict name, uintptr_t id, vkaa_type_create_f create)
{
	vkaa_type_s *restrict r, *rr;
	rr = NULL;
	if ((r = (vkaa_type_s *) refer_alloz(sizeof(vkaa_type_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_type_finally);
		if (vkaa_type_initial(r, id, name, create) && vkaa_tpool_insert(tpool, r))
			rr = r;
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
		if (
			vkaa_std_tpool_set_void(r, typeid->id_void) &&
			vkaa_std_tpool_set_scope(r, typeid->id_scope) &&
			vkaa_tpool_var_const_enable_by_id(r, typeid->id_void)
		) return r;
		refer_free(r);
	}
	return NULL;
}
