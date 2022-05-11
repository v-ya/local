#include "std.tpool.h"

vkaa_type_s* vkaa_std_tpool_set(vkaa_tpool_s *restrict tpool, const char *restrict name, uintptr_t id, vkaa_type_create_f create, vkaa_std_type_initial_f initial, vkaa_std_typeid_s *restrict typeid)
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

static vkaa_error_s* vkaa_std_create_tpool_initial_error(vkaa_error_s *restrict error)
{
	if (
		vkaa_error_add_error(error, vkaa_std_error_memory_less) &&
		vkaa_error_add_error(error, vkaa_std_error_div_zero) &&
		vkaa_error_add_error(error, vkaa_std_error_function_empty) &&
		vkaa_error_add_error(error, vkaa_std_error_function_stack) &&
		vkaa_error_add_error(error, vkaa_std_error_function_initial)
	) return error;
	return NULL;
}

vkaa_tpool_s* vkaa_std_create_tpool(vkaa_std_typeid_s *restrict *restrict typeid)
{
	vkaa_tpool_s *restrict r;
	vkaa_std_typeid_s *restrict tid;
	if ((r = vkaa_tpool_alloc()))
	{
		if (
			!*typeid &&
			(*typeid = tid = vkaa_std_typeid_alloc(r))&&
			vkaa_std_create_tpool_initial_error(r->e) &&
			vkaa_std_tpool_set_void(r, tid) &&
			vkaa_std_tpool_set_null(r, tid) &&
			vkaa_std_tpool_set_scope(r, tid) &&
			vkaa_std_tpool_set_syntax(r, tid) &&
			vkaa_std_tpool_set_marco(r, tid) &&
			vkaa_std_tpool_set_function(r, tid) &&
			vkaa_std_tpool_set_string(r, tid) &&
			vkaa_std_tpool_set_bool(r, tid) &&
			vkaa_std_tpool_set_uint(r, tid) &&
			vkaa_std_tpool_set_int(r, tid) &&
			vkaa_std_tpool_set_float(r, tid) &&
			vkaa_tpool_var_const_enable_by_id(r, tid->id_void) &&
			vkaa_tpool_var_const_enable_by_id(r, tid->id_null)
		) return r;
		refer_free(r);
	}
	return NULL;
}
