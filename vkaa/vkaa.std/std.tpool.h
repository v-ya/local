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
#define vkaa_std_function_define(_type, _name)  vkaa_var_s* vkaa_std_function_label(_type, _name)(const vkaa_function_s *restrict r);

#endif
