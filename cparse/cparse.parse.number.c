#include "cparse.type.h"

typedef struct cparse_parse_number_s {
	cparse_parse_s parse;
	tparse_tstring_s *ts;
	cparse_value_s *operator;
} cparse_parse_number_s;

static cparse_parse_s* cparse_inner_parse_number(cparse_parse_number_s *restrict pri, cparse_inst_s *restrict inst, cparse_inst_context_t *restrict context)
{
	cparse_parse_s *ret;
	tparse_tstring_s *ts;
	ret = NULL;
	ts = pri->ts;
	tparse_tstring_clear(ts);
	if (tparse_tstring_transport(ts, context->data, context->size, context->pos - 1, &context->pos))
	{
		cparse_value_s *restrict v;
		const char *restrict string;
		uintptr_t length;
		string = tparse_tstring_get_data(ts, &length);
		if (length != 3 || string[0] != '.' || string[1] != '.' || string[2] != '.')
		{
			v = cparse_inner_alloc_value_nstring(cparse_value$number, string, length);
			if (v)
			{
				if (vattr_insert_tail(context->scope, "0", v))
					ret = &pri->parse;
				refer_free(v);
			}
		}
		else if (vattr_insert_tail(context->scope, "...", pri->operator))
			ret = &pri->parse;
	}
	return ret;
}

static void cparse_parse_number_free_func(cparse_parse_number_s *restrict r)
{
	if (r->ts)
		refer_free(r->ts);
	if (r->operator)
		refer_free(r->operator);
}

cparse_parse_s* cparse_inner_alloc_parse_number(void)
{
	cparse_parse_number_s *restrict r;
	if ((r = (cparse_parse_number_s *) refer_alloz(sizeof(cparse_parse_number_s))))
	{
		refer_set_free(r, (refer_free_f) cparse_parse_number_free_func);
		r->parse.parse = (cparse_parse_f) cparse_inner_parse_number;
		if ((r->ts = cparse_inner_alloc_tstring_number()) &&
			(r->operator = cparse_inner_alloc_value_empty(cparse_value$operator)))
			return &r->parse;
		refer_free(r);
	}
	return NULL;
}
