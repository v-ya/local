#ifndef _vkaa_std_std_parse_h_
#define _vkaa_std_std_parse_h_

#include "std.h"

// keyword

vkaa_parse_keyword_s* vkaa_std_parse_set_keyword_var(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid);
vkaa_parse_keyword_s* vkaa_std_parse_set_keyword_func(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid);
vkaa_parse_keyword_s* vkaa_std_parse_set_keyword_label(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid);
vkaa_parse_keyword_s* vkaa_std_parse_set_keyword_goto(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid);
vkaa_parse_keyword_s* vkaa_std_parse_set_keyword_break(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid);
vkaa_parse_keyword_s* vkaa_std_parse_set_keyword_continue(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid);
vkaa_parse_keyword_s* vkaa_std_parse_set_keyword_return(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid);
vkaa_parse_keyword_s* vkaa_std_parse_set_keyword_if(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid);
vkaa_parse_keyword_s* vkaa_std_parse_set_keyword_else(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid);
vkaa_parse_keyword_s* vkaa_std_parse_set_keyword_while(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid);
vkaa_parse_keyword_s* vkaa_std_parse_set_keyword_do(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid);
vkaa_parse_keyword_s* vkaa_std_parse_set_keyword_for(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid);
vkaa_parse_keyword_s* vkaa_std_parse_set_keyword_null(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid);
vkaa_parse_keyword_s* vkaa_std_parse_set_keyword_true(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid);
vkaa_parse_keyword_s* vkaa_std_parse_set_keyword_false(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid);

// operator

// type2var

vkaa_parse_type2var_s* vkaa_std_parse_set_type2var_scope(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid);
vkaa_parse_type2var_s* vkaa_std_parse_set_type2var_brackets(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid);
vkaa_parse_type2var_s* vkaa_std_parse_set_type2var_square(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid);
vkaa_parse_type2var_s* vkaa_std_parse_set_type2var_keyword(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid);
vkaa_parse_type2var_s* vkaa_std_parse_set_type2var_string(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid);
vkaa_parse_type2var_s* vkaa_std_parse_set_type2var_number(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid);

#define vkaa_std_keyword_label(_name)    vkaa_std_keyword__##_name
#define vkaa_std_operator_label(_name)   vkaa_std_operator__##_name
#define vkaa_std_type2var_label(_name)   vkaa_std_type2var__##_name
#define vkaa_std_keyword_define(_name)   vkaa_parse_result_t* vkaa_std_keyword_label(_name)(const vkaa_std_keyword_s *restrict r, vkaa_parse_result_t *restrict result, const vkaa_parse_context_t *restrict context, vkaa_parse_syntax_t *restrict syntax)
#define vkaa_std_operator_define(_name)  vkaa_parse_result_t* vkaa_std_operator_label(_name)(const vkaa_std_operator_s *restrict r, vkaa_parse_result_t *restrict result, const vkaa_parse_context_t *restrict context, const vkaa_syntax_t *restrict syntax, const vkaa_parse_result_t *const param_list)
#define vkaa_std_type2var_define(_name)  vkaa_parse_result_t* vkaa_std_type2var_label(_name)(const vkaa_std_type2var_s *restrict r, vkaa_parse_result_t *restrict result, const vkaa_parse_context_t *restrict context, const vkaa_syntax_t *restrict syntax)

#endif
