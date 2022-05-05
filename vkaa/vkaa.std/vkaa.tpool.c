#include "std.tpool.h"

vkaa_type_s* vkaa_std_tpool_set(vkaa_tpool_s *restrict tpool, const char *restrict name, uintptr_t id, vkaa_type_create_f create, vkaa_std_type_initial_f initial, const vkaa_std_typeid_t *restrict typeid)
{
	vkaa_type_s *restrict r, *rr;
	rr = NULL;
	if ((r = (vkaa_type_s *) refer_alloz(sizeof(vkaa_type_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_type_finally);
		if (vkaa_type_initial(r, id, name, create) &&
			(!initial || initial(r, typeid)) &&
			vkaa_tpool_insert(tpool, r))
			rr = r;
		refer_free(r);
	}
	return rr;
}

vkaa_std_selector_s* vkaa_std_type_set_function(vkaa_type_s *restrict type, const char *restrict name, vkaa_function_f function, vkaa_std_selector_output_t output, uintptr_t this_typeid, uintptr_t output_typeid, uintptr_t input_number, const uintptr_t *restrict input_typeid)
{
	vkaa_std_selector_s *s, *snew;
	if (!vkaa_type_find_selector(type, name) && (snew = vkaa_std_selector_alloc()))
	{
		s = (vkaa_std_selector_s *) vkaa_type_insert_selector(type, name, &snew->selector);
		refer_free(snew);
		if (s && vkaa_std_selector_append(s, name, function, output, this_typeid, output_typeid, input_number, input_typeid))
			return s;
	}
	return NULL;
}

static void vkaa_std_create_tpool_initial_id(vkaa_std_typeid_t *restrict typeid, vkaa_tpool_s *restrict tpool)
{
	typeid->id_void = vkaa_tpool_genid(tpool);
	typeid->id_null = vkaa_tpool_genid(tpool);
	typeid->id_scope = vkaa_tpool_genid(tpool);
	typeid->id_syntax = vkaa_tpool_genid(tpool);
	typeid->id_function = vkaa_tpool_genid(tpool);
	typeid->id_bool = vkaa_tpool_genid(tpool);
	typeid->id_string = vkaa_tpool_genid(tpool);
	typeid->id_uint = vkaa_tpool_genid(tpool);
	typeid->id_int = vkaa_tpool_genid(tpool);
	typeid->id_float = vkaa_tpool_genid(tpool);
}

vkaa_tpool_s* vkaa_std_create_tpool(vkaa_std_typeid_t *restrict typeid)
{
	vkaa_tpool_s *restrict r;
	if ((r = vkaa_tpool_alloc()))
	{
		vkaa_std_create_tpool_initial_id(typeid, r);
		if (
			vkaa_std_tpool_set_void(r, typeid) &&
			vkaa_std_tpool_set_null(r, typeid) &&
			vkaa_std_tpool_set_scope(r, typeid) &&
			vkaa_std_tpool_set_syntax(r, typeid) &&
			vkaa_std_tpool_set_function(r, typeid) &&
			vkaa_std_tpool_set_bool(r, typeid) &&
			vkaa_std_tpool_set_string(r, typeid) &&
			vkaa_std_tpool_set_uint(r, typeid) &&
			vkaa_std_tpool_set_int(r, typeid) &&
			vkaa_std_tpool_set_float(r, typeid) &&
			vkaa_tpool_var_const_enable_by_id(r, typeid->id_void) &&
			vkaa_tpool_var_const_enable_by_id(r, typeid->id_null)
		) return r;
		refer_free(r);
	}
	return NULL;
}
