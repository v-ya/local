#include "std.h"

vkaa_oplevel_s* vkaa_std_create_oplevel(void)
{
	vkaa_oplevel_s *restrict r;
	if ((r = vkaa_oplevel_alloc()))
	{
		if (
			vkaa_oplevel_insert_tail(r, vkaa_std_oplevel_assign)    && // = += -= *= /= %= **= &= |= ^= <<= >>=
			vkaa_oplevel_insert_tail(r, vkaa_std_oplevel_condition) && // ?:
			vkaa_oplevel_insert_tail(r, vkaa_std_oplevel_relation)  && // == != > < >= <=
			vkaa_oplevel_insert_tail(r, vkaa_std_oplevel_logic)     && // && ||
			vkaa_oplevel_insert_tail(r, vkaa_std_oplevel_bitwise)   && // & | ^ << >>
			vkaa_oplevel_insert_tail(r, vkaa_std_oplevel_arith_1)   && // + -
			vkaa_oplevel_insert_tail(r, vkaa_std_oplevel_arith_2)   && // * / %
			vkaa_oplevel_insert_tail(r, vkaa_std_oplevel_arith_3)   && // **
			vkaa_oplevel_insert_tail(r, vkaa_std_oplevel_unary)     && // ! ~ ++ -- -
			vkaa_oplevel_insert_tail(r, vkaa_std_oplevel_inquiry)      // . [] ()
		) return r;
		refer_free(r);
	}
	return NULL;
}
