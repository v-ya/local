#include "cparse.type.h"
#include <memory.h>

static uintptr_t cparse_inner_parse_comment_block_find_end(const char *restrict data, uintptr_t size, uintptr_t pos)
{
	for (++pos; pos < size; ++pos)
	{
		if (data[pos - 1] == '*' && data[pos] == '/')
			return pos + 1;
	}
	return 0;
}

cparse_parse_s* cparse_inner_parse_comment_line(cparse_parse_s *restrict pri, cparse_inst_s *restrict inst, cparse_inst_context_t *restrict context)
{
	void *p;
	uintptr_t pos, n;
	cparse_value_s *v;
	cparse_parse_s *ret;
	ret = NULL;
	pos = context->pos;
	if ((p = memchr(context->data + pos, '\n', context->size - pos)))
	{
		context->pos = (uintptr_t) p - (uintptr_t) context->data + 1;
		n = (uintptr_t) p - (uintptr_t) context->data - pos;
		if (n && context->data[n - 1] == '\r')
			--n;
	}
	else
	{
		context->pos = context->size;
		n = context->size - pos;
	}
	v = cparse_inner_alloc_value_nstring(cparse_value$comment, context->data + pos, n);
	if (v)
	{
		if (vattr_insert_tail(context->scope, "//", v))
			ret = pri;
		refer_free(v);
	}
	return ret;
}

cparse_parse_s* cparse_inner_parse_comment_block(cparse_parse_s *restrict pri, cparse_inst_s *restrict inst, cparse_inst_context_t *restrict context)
{
	uintptr_t pos, p;
	cparse_value_s *v;
	cparse_parse_s *ret;
	ret = NULL;
	if ((p = cparse_inner_parse_comment_block_find_end(context->data, context->size, pos = context->pos)))
	{
		context->pos = p;
		p -= 2;
	}
	else
	{
		context->pos = context->size;
		p = context->size;
	}
	v = cparse_inner_alloc_value_nstring(cparse_value$comment, context->data + pos, p - pos);
	if (v)
	{
		if (vattr_insert_tail(context->scope, "/*", v))
			ret = pri;
		refer_free(v);
	}
	return ret;
}
