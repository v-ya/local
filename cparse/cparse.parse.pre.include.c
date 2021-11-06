#include "cparse.type.h"
#include <memory.h>

cparse_parse_s* cparse_inner_parse_pre_include(cparse_parse_s *restrict pri, cparse_inst_s *restrict inst, cparse_inst_context_t *restrict context)
{
	const char *p_sys, *p_cur;
	uintptr_t n;
	cparse_value_s *restrict value;
	cparse_parse_s *ret;
	ret = NULL;
	value = NULL;
	p_sys = memchr(context->data + context->pos, '<', context->size - context->pos);
	p_cur = memchr(context->data + context->pos, '\"', context->size - context->pos);
	if (p_sys && (!p_cur || p_sys <= p_cur))
	{
		// <>
		++p_sys;
		n = (uintptr_t) context->data + context->size - (uintptr_t) p_sys;
		if ((p_cur = memchr(p_sys, '>', n)))
			n = (uintptr_t) p_cur - (uintptr_t) p_sys;
		value = cparse_inner_alloc_value_nstring(cparse_value$include$sys, p_sys, n);
		context->pos = (uintptr_t) p_sys - (uintptr_t) context->data + n;
	}
	else if (p_cur)
	{
		// ""
		++p_cur;
		n = (uintptr_t) context->data + context->size - (uintptr_t) p_cur;
		if ((p_sys = memchr(p_cur, '\"', n)))
			n = (uintptr_t) p_sys - (uintptr_t) p_cur;
		value = cparse_inner_alloc_value_nstring(cparse_value$include$sys, p_cur, n);
		context->pos = (uintptr_t) p_cur - (uintptr_t) context->data + n;
	}
	else
	{
		value = cparse_inner_alloc_value_nstring(cparse_value$pretreat, context->data + context->pos, context->size - context->pos);
		context->pos = context->size;
	}
	if (value)
	{
		if (vattr_insert_tail(context->scope, "#include", value))
			ret = pri;
		refer_free(value);
	}
	return ret;
}
