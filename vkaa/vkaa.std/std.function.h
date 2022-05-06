#ifndef _vkaa_std_std_function_h_
#define _vkaa_std_std_function_h_

#include "std.h"

#define vkaa_std_function_label(_type, _name)   vkaa_std_function__##_type##_##_name
#define vkaa_std_function_define(_type, _name)  uintptr_t vkaa_std_function_label(_type, _name)(const vkaa_function_s *restrict r)

#define vkaa_std_vp(_type, _i)  (((vkaa_std_var_##_type##_s *) r->input_list[_i])->value)
#define vkaa_std_vo(_type)      (((vkaa_std_var_##_type##_s *) r->output)->value)
#define vkaa_std_vt(_type)      (((vkaa_std_var_##_type##_s *) r->this)->value)

#define vkaa_std_error(_e)  vkaa_error_get_id(r->output->type->error, vkaa_std_error_##_e);

vkaa_std_selector_s* vkaa_std_type_set_function(vkaa_type_s *restrict type, const char *restrict name, vkaa_function_f function, vkaa_std_selector_output_t output, vkaa_std_selector_convert_t convert, uintptr_t this_typeid, uintptr_t output_typeid, uintptr_t input_number, const uintptr_t *restrict input_typeid);

// *
vkaa_std_function_define(uint, op_mul);
vkaa_std_function_define(int, op_mul);
vkaa_std_function_define(float, op_mul);
// /
vkaa_std_function_define(uint, op_div);
vkaa_std_function_define(int, op_div);
vkaa_std_function_define(float, op_div);
// +
vkaa_std_function_define(uint, op_add);
vkaa_std_function_define(int, op_add);
vkaa_std_function_define(float, op_add);
vkaa_std_function_define(uint, op_pos);
vkaa_std_function_define(int, op_pos);
vkaa_std_function_define(float, op_pos);
// -
vkaa_std_function_define(uint, op_sub);
vkaa_std_function_define(int, op_sub);
vkaa_std_function_define(float, op_sub);
vkaa_std_function_define(uint, op_neg);
vkaa_std_function_define(int, op_neg);
vkaa_std_function_define(float, op_neg);
// =
vkaa_std_function_define(bool, op_mov);
vkaa_std_function_define(uint, op_mov);
vkaa_std_function_define(int, op_mov);
vkaa_std_function_define(float, op_mov);

// conver<bool>
vkaa_std_function_define(bool, cv_bool);
vkaa_std_function_define(bool, cv_uint);
vkaa_std_function_define(bool, cv_int);
vkaa_std_function_define(bool, cv_float);
// conver<uint>
vkaa_std_function_define(uint, cv_bool);
vkaa_std_function_define(uint, cv_uint);
vkaa_std_function_define(uint, cv_int);
vkaa_std_function_define(uint, cv_float);
// conver<int>
vkaa_std_function_define(int, cv_bool);
vkaa_std_function_define(int, cv_uint);
vkaa_std_function_define(int, cv_int);
vkaa_std_function_define(int, cv_float);
// conver<float>
vkaa_std_function_define(float, cv_bool);
vkaa_std_function_define(float, cv_uint);
vkaa_std_function_define(float, cv_int);
vkaa_std_function_define(float, cv_float);

#endif
