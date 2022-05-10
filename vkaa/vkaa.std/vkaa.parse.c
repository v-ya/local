#include "std.parse.h"

static vkaa_parse_s* vkaa_std_create_parse_preset_operator(vkaa_parse_s *restrict r, vkaa_std_typeid_s *restrict typeid, const vkaa_oplevel_s *restrict opl)
{
	if (
		#define d_op(_t, _op, _lev, _tow)  vkaa_std_parse_set_operator_##_t(r, typeid, opl, _op, vkaa_std_oplevel_##_lev, vkaa_parse_optowards_##_tow)
		d_op(unary_left, vkaa_parse_operator_brackets, inquiry, left2right) &&
		d_op(unary_left, vkaa_parse_operator_square,   inquiry, left2right) &&
		d_op(binary_selector,   ".",   inquiry,   left2right) &&
		d_op(unary_right,       "+",   unary,     right2left) &&
		d_op(unary_right,       "-",   unary,     right2left) &&
		d_op(unary_right,       "!",   unary,     right2left) &&
		d_op(unary_right,       "~",   unary,     right2left) &&
		d_op(binary,            "**",  arith_3,   left2right) &&
		d_op(binary,            "*",   arith_2,   left2right) &&
		d_op(binary,            "/",   arith_2,   left2right) &&
		d_op(binary,            "%",   arith_2,   left2right) &&
		d_op(binary,            "+",   arith_1,   left2right) &&
		d_op(binary,            "-",   arith_1,   left2right) &&
		d_op(binary,            "&",   bitwise,   left2right) &&
		d_op(binary,            "|",   bitwise,   left2right) &&
		d_op(binary,            "^",   bitwise,   left2right) &&
		d_op(binary,            "<<",  bitwise,   left2right) &&
		d_op(binary,            ">>",  bitwise,   left2right) &&
		d_op(binary_testeval,   "&&",  logic,     left2right) &&
		d_op(binary_testeval,   "||",  logic,     left2right) &&
		d_op(binary,            "^^",  logic,     left2right) &&
		d_op(binary,            "==",  relation,  left2right) &&
		d_op(binary,            "!=",  relation,  left2right) &&
		d_op(binary,            ">",   relation,  left2right) &&
		d_op(binary,            "<",   relation,  left2right) &&
		d_op(binary,            ">=",  relation,  left2right) &&
		d_op(binary,            "<=",  relation,  left2right) &&
		vkaa_std_parse_set_operator_ternary_testeval(r, typeid, opl, "?", ":", vkaa_std_oplevel_condition, vkaa_parse_optowards_right2left) &&
		d_op(binary_assign,     "=",   assign,    right2left) &&
		d_op(binary,            "+=",  assign,    right2left) &&
		d_op(binary,            "-=",  assign,    right2left) &&
		d_op(binary,            "*=",  assign,    right2left) &&
		d_op(binary,            "/=",  assign,    right2left) &&
		d_op(binary,            "%=",  assign,    right2left) &&
		d_op(binary,            "**=", assign,    right2left) &&
		d_op(binary,            "&=",  assign,    right2left) &&
		d_op(binary,            "|=",  assign,    right2left) &&
		d_op(binary,            "^=",  assign,    right2left) &&
		d_op(binary,            "<<=", assign,    right2left) &&
		d_op(binary,            ">>=", assign,    right2left)
		#undef d_op
	) return r;
	return NULL;
}

vkaa_parse_s* vkaa_std_create_parse(const vkaa_oplevel_s *restrict oplevel, vkaa_std_typeid_s *restrict typeid)
{
	vkaa_parse_s *restrict r;
	if ((r = vkaa_parse_alloc()))
	{
		if (
			vkaa_std_parse_set_keyword_var(r, typeid) &&
			vkaa_std_parse_set_keyword_func(r, typeid) &&
			vkaa_std_parse_set_keyword_label(r, typeid) &&
			vkaa_std_parse_set_keyword_goto(r, typeid) &&
			vkaa_std_parse_set_keyword_break(r, typeid) &&
			vkaa_std_parse_set_keyword_continue(r, typeid) &&
			vkaa_std_parse_set_keyword_return(r, typeid) &&
			vkaa_std_parse_set_keyword_if(r, typeid) &&
			vkaa_std_parse_set_keyword_else(r, typeid) &&
			vkaa_std_parse_set_keyword_while(r, typeid) &&
			vkaa_std_parse_set_keyword_do(r, typeid) &&
			vkaa_std_parse_set_keyword_for(r, typeid) &&
			vkaa_std_parse_set_keyword_null(r, typeid) &&
			vkaa_std_parse_set_keyword_true(r, typeid) &&
			vkaa_std_parse_set_keyword_false(r, typeid) &&
			vkaa_std_create_parse_preset_operator(r, typeid, oplevel) &&
			vkaa_std_parse_set_type2var_scope(r, typeid) &&
			vkaa_std_parse_set_type2var_brackets(r, typeid) &&
			vkaa_std_parse_set_type2var_square(r, typeid) &&
			vkaa_std_parse_set_type2var_keyword(r, typeid) &&
			vkaa_std_parse_set_type2var_string(r, typeid) &&
			vkaa_std_parse_set_type2var_number(r, typeid)
		) return r;
		refer_free(r);
	}
	return NULL;
}
