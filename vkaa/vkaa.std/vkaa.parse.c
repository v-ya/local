#include "std.parse.h"

static vkaa_parse_s* vkaa_std_create_parse_preset_operator(vkaa_parse_s *restrict r, const vkaa_oplevel_s *restrict opl)
{
	if (
		vkaa_std_parse_set_operator_unary_left(r, opl, vkaa_parse_operator_brackets, vkaa_std_oplevel_inquiry, vkaa_parse_optowards_left2right) &&
		vkaa_std_parse_set_operator_unary_left(r, opl, vkaa_parse_operator_square, vkaa_std_oplevel_inquiry, vkaa_parse_optowards_left2right) &&
		vkaa_std_parse_set_operator_binary_second_type2var(r, opl, ".", vkaa_std_oplevel_inquiry, vkaa_parse_optowards_left2right) &&
		vkaa_std_parse_set_operator_unary_right(r, opl, "!", vkaa_std_oplevel_unary, vkaa_parse_optowards_right2left) &&
		vkaa_std_parse_set_operator_unary_right(r, opl, "~", vkaa_std_oplevel_unary, vkaa_parse_optowards_right2left) &&
		vkaa_std_parse_set_operator_binary(r, opl, "**", vkaa_std_oplevel_arith_3, vkaa_parse_optowards_left2right) &&
		vkaa_std_parse_set_operator_binary(r, opl, "*", vkaa_std_oplevel_arith_2, vkaa_parse_optowards_left2right) &&
		vkaa_std_parse_set_operator_binary(r, opl, "/", vkaa_std_oplevel_arith_2, vkaa_parse_optowards_left2right) &&
		vkaa_std_parse_set_operator_binary(r, opl, "%", vkaa_std_oplevel_arith_2, vkaa_parse_optowards_left2right) &&
		vkaa_std_parse_set_operator_binary_or_unary_right(r, opl, "+", vkaa_std_oplevel_arith_1, vkaa_parse_optowards_left2right) &&
		vkaa_std_parse_set_operator_binary_or_unary_right(r, opl, "-", vkaa_std_oplevel_arith_1, vkaa_parse_optowards_left2right) &&
		vkaa_std_parse_set_operator_binary(r, opl, "&", vkaa_std_oplevel_bitwise, vkaa_parse_optowards_left2right) &&
		vkaa_std_parse_set_operator_binary(r, opl, "|", vkaa_std_oplevel_bitwise, vkaa_parse_optowards_left2right) &&
		vkaa_std_parse_set_operator_binary(r, opl, "^", vkaa_std_oplevel_bitwise, vkaa_parse_optowards_left2right) &&
		vkaa_std_parse_set_operator_binary(r, opl, "<<", vkaa_std_oplevel_bitwise, vkaa_parse_optowards_left2right) &&
		vkaa_std_parse_set_operator_binary(r, opl, ">>", vkaa_std_oplevel_bitwise, vkaa_parse_optowards_left2right) &&
		vkaa_std_parse_set_operator_binary(r, opl, "&&", vkaa_std_oplevel_logic, vkaa_parse_optowards_left2right) &&
		vkaa_std_parse_set_operator_binary(r, opl, "||", vkaa_std_oplevel_logic, vkaa_parse_optowards_left2right) &&
		vkaa_std_parse_set_operator_binary(r, opl, "^^", vkaa_std_oplevel_logic, vkaa_parse_optowards_left2right) &&
		vkaa_std_parse_set_operator_binary(r, opl, "==", vkaa_std_oplevel_relation, vkaa_parse_optowards_left2right) &&
		vkaa_std_parse_set_operator_binary(r, opl, "!=", vkaa_std_oplevel_relation, vkaa_parse_optowards_left2right) &&
		vkaa_std_parse_set_operator_binary(r, opl, ">", vkaa_std_oplevel_relation, vkaa_parse_optowards_left2right) &&
		vkaa_std_parse_set_operator_binary(r, opl, "<", vkaa_std_oplevel_relation, vkaa_parse_optowards_left2right) &&
		vkaa_std_parse_set_operator_binary(r, opl, ">=", vkaa_std_oplevel_relation, vkaa_parse_optowards_left2right) &&
		vkaa_std_parse_set_operator_binary(r, opl, "<=", vkaa_std_oplevel_relation, vkaa_parse_optowards_left2right) &&
		// vkaa_std_parse_set_operator_ternary(r, opl, "?", ":", vkaa_std_oplevel_condition, vkaa_parse_optowards_right2left, NULL) &&
		vkaa_std_parse_set_operator_binary_assign(r, opl, "=", vkaa_std_oplevel_assign, vkaa_parse_optowards_right2left) &&
		vkaa_std_parse_set_operator_binary(r, opl, "+=", vkaa_std_oplevel_assign, vkaa_parse_optowards_right2left) &&
		vkaa_std_parse_set_operator_binary(r, opl, "-=", vkaa_std_oplevel_assign, vkaa_parse_optowards_right2left) &&
		vkaa_std_parse_set_operator_binary(r, opl, "*=", vkaa_std_oplevel_assign, vkaa_parse_optowards_right2left) &&
		vkaa_std_parse_set_operator_binary(r, opl, "/=", vkaa_std_oplevel_assign, vkaa_parse_optowards_right2left) &&
		vkaa_std_parse_set_operator_binary(r, opl, "%=", vkaa_std_oplevel_assign, vkaa_parse_optowards_right2left) &&
		vkaa_std_parse_set_operator_binary(r, opl, "**=", vkaa_std_oplevel_assign, vkaa_parse_optowards_right2left) &&
		vkaa_std_parse_set_operator_binary(r, opl, "&=", vkaa_std_oplevel_assign, vkaa_parse_optowards_right2left) &&
		vkaa_std_parse_set_operator_binary(r, opl, "|=", vkaa_std_oplevel_assign, vkaa_parse_optowards_right2left) &&
		vkaa_std_parse_set_operator_binary(r, opl, "^=", vkaa_std_oplevel_assign, vkaa_parse_optowards_right2left) &&
		vkaa_std_parse_set_operator_binary(r, opl, "<<=", vkaa_std_oplevel_assign, vkaa_parse_optowards_right2left) &&
		vkaa_std_parse_set_operator_binary(r, opl, ">>=", vkaa_std_oplevel_assign, vkaa_parse_optowards_right2left)
	) return r;
	return NULL;
}

vkaa_parse_s* vkaa_std_create_parse(const vkaa_oplevel_s *restrict oplevel, const vkaa_std_typeid_t *restrict typeid)
{
	vkaa_parse_s *restrict r;
	if ((r = vkaa_parse_alloc()))
	{
		if (
			vkaa_std_parse_set_keyword_var(r, typeid) &&
			vkaa_std_parse_set_keyword_null(r, typeid) &&
			vkaa_std_create_parse_preset_operator(r, oplevel) &&
			vkaa_std_parse_set_type2var_scope(r, typeid) &&
			vkaa_std_parse_set_type2var_brackets(r, typeid) &&
			vkaa_std_parse_set_type2var_square(r, typeid)
		) return r;
		refer_free(r);
	}
	return NULL;
}
