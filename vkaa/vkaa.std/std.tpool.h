#ifndef _vkaa_std_std_tpool_h_
#define _vkaa_std_std_tpool_h_

#include "std.h"

vkaa_type_s* vkaa_std_tpool_set_void(vkaa_tpool_s *restrict tpool, vkaa_std_typeid_s *restrict typeid);
vkaa_type_s* vkaa_std_tpool_set_null(vkaa_tpool_s *restrict tpool, vkaa_std_typeid_s *restrict typeid);
vkaa_type_s* vkaa_std_tpool_set_scope(vkaa_tpool_s *restrict tpool, vkaa_std_typeid_s *restrict typeid);
vkaa_type_s* vkaa_std_tpool_set_syntax(vkaa_tpool_s *restrict tpool, vkaa_std_typeid_s *restrict typeid);
vkaa_type_s* vkaa_std_tpool_set_marco(vkaa_tpool_s *restrict tpool, vkaa_std_typeid_s *restrict typeid);
vkaa_type_s* vkaa_std_tpool_set_function(vkaa_tpool_s *restrict tpool, vkaa_std_typeid_s *restrict typeid);
vkaa_type_s* vkaa_std_tpool_set_refer(vkaa_tpool_s *restrict tpool, vkaa_std_typeid_s *restrict typeid);
vkaa_type_s* vkaa_std_tpool_set_string(vkaa_tpool_s *restrict tpool, vkaa_std_typeid_s *restrict typeid);
vkaa_type_s* vkaa_std_tpool_set_bool(vkaa_tpool_s *restrict tpool, vkaa_std_typeid_s *restrict typeid);
vkaa_type_s* vkaa_std_tpool_set_uint(vkaa_tpool_s *restrict tpool, vkaa_std_typeid_s *restrict typeid);
vkaa_type_s* vkaa_std_tpool_set_int(vkaa_tpool_s *restrict tpool, vkaa_std_typeid_s *restrict typeid);
vkaa_type_s* vkaa_std_tpool_set_float(vkaa_tpool_s *restrict tpool, vkaa_std_typeid_s *restrict typeid);
vkaa_type_s* vkaa_std_tpool_set_bytes(vkaa_tpool_s *restrict tpool, vkaa_std_typeid_s *restrict typeid);

#define vkaa_std_type_init_label(_name)     vkaa_std_type__##_name##_init
#define vkaa_std_type_create_label(_name)   vkaa_std_type__##_name##_create
#define vkaa_std_type_clear_label(_name)    vkaa_std_type__##_name##_clear
#define vkaa_std_type_init_define(_name)    vkaa_type_s* vkaa_std_type_init_label(_name)(vkaa_type_s *restrict type, vkaa_std_typeid_s *restrict typeid)
#define vkaa_std_type_create_define(_name)  vkaa_var_s* vkaa_std_type_create_label(_name)(const vkaa_type_s *restrict type, const vkaa_syntax_s *restrict syntax)
#define vkaa_std_type_clear_define(_name)   void vkaa_std_type_clear_label(_name)(vkaa_var_s *restrict var)

#define vkaa_std_selector_label(_type, _name)   vkaa_std_selector__##_type##_##_name
#define vkaa_std_selector_define(_type, _name)  vkaa_function_s* vkaa_std_selector_label(_type, _name)(const vkaa_selector_s *restrict r, const vkaa_selector_param_t *restrict param)

#define sf_need(_l, _tt, _to, _ai, _n, _o, _c)  vkaa_std_function_label(_tt, _l), _o, _c, typeid->id_##_tt, typeid->id_##_to, _n, _ai
#define sf_need0cvn(_tt, _to, _l)               sf_need(_l, _tt, _to, NULL, 0, vkaa_std_selector_output_any, vkaa_std_selector_convert_none)
#define sf_need0cvp(_tt, _to, _l)               sf_need(_l, _tt, _to, NULL, 0, vkaa_std_selector_output_any, vkaa_std_selector_convert_promotion)
#define sfsi_need(_l, _to, _ti, _n, _o)         vkaa_std_function_label(_ti, _l), _o, vkaa_std_selector_convert_none, 0, typeid->id_##_to, _n, typeid->id_##_ti
#define sfsi_need1a(_t, _l)                     sfsi_need(_l, _t, _t, 1, vkaa_std_selector_output_any)
#define sfsi_need1i(_t, _l)                     sfsi_need(_l, _t, _t, 1, vkaa_std_selector_output_input_first)
#define sfsi_need2a(_t, _l)                     sfsi_need(_l, _t, _t, 2, vkaa_std_selector_output_any)
#define sfsi_need2r(_t, _l)                     sfsi_need(_l, bool, _t, 2, vkaa_std_selector_output_any)
#define sfsi_need2m(_t, _l)                     sfsi_need(_l, _t, _t, 2, vkaa_std_selector_output_must_first)

#include "std.function.h"

#endif
