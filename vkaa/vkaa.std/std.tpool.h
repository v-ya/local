#ifndef _vkaa_std_std_tpool_h_
#define _vkaa_std_std_tpool_h_

#include "std.h"

vkaa_type_s* vkaa_std_tpool_set_void(vkaa_tpool_s *restrict tpool, const vkaa_std_typeid_t *restrict typeid);
vkaa_type_s* vkaa_std_tpool_set_null(vkaa_tpool_s *restrict tpool, const vkaa_std_typeid_t *restrict typeid);
vkaa_type_s* vkaa_std_tpool_set_scope(vkaa_tpool_s *restrict tpool, const vkaa_std_typeid_t *restrict typeid);
vkaa_type_s* vkaa_std_tpool_set_syntax(vkaa_tpool_s *restrict tpool, const vkaa_std_typeid_t *restrict typeid);
vkaa_type_s* vkaa_std_tpool_set_function(vkaa_tpool_s *restrict tpool, const vkaa_std_typeid_t *restrict typeid);
vkaa_type_s* vkaa_std_tpool_set_bool(vkaa_tpool_s *restrict tpool, const vkaa_std_typeid_t *restrict typeid);
vkaa_type_s* vkaa_std_tpool_set_string(vkaa_tpool_s *restrict tpool, const vkaa_std_typeid_t *restrict typeid);
vkaa_type_s* vkaa_std_tpool_set_uint(vkaa_tpool_s *restrict tpool, const vkaa_std_typeid_t *restrict typeid);
vkaa_type_s* vkaa_std_tpool_set_int(vkaa_tpool_s *restrict tpool, const vkaa_std_typeid_t *restrict typeid);
vkaa_type_s* vkaa_std_tpool_set_float(vkaa_tpool_s *restrict tpool, const vkaa_std_typeid_t *restrict typeid);

#define vkaa_std_function_label(_type, _name)   vkaa_std_function__##_type##_##_name
#define vkaa_std_function_define(_type, _name)  vkaa_var_s* vkaa_std_function_label(_type, _name)(const vkaa_function_s *restrict r)

#define vkaa_std_vp(_type, _i)  (((vkaa_std_var_##_type##_s *) r->input_list[_i])->value)
#define vkaa_std_vo(_type)      (((vkaa_std_var_##_type##_s *) r->output)->value)
#define vkaa_std_vt(_type)      (((vkaa_std_var_##_type##_s *) r->this)->value)

vkaa_std_selector_s* vkaa_std_type_set_function(vkaa_type_s *restrict type, const char *restrict name, vkaa_function_f function, vkaa_std_selector_output_t output, uintptr_t this_typeid, uintptr_t output_typeid, uintptr_t input_number, const uintptr_t *restrict input_typeid);

#endif
