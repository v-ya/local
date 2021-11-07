#include "cparse.type.h"

typedef struct cparse_parse_string_s {
	cparse_parse_s parse;
	tparse_tstring_s *ts;
	const char *key;
	cparse_value_type_t type;
} cparse_parse_string_s;

static void cparse_parse_string_free_func(cparse_parse_string_s *restrict r)
{
	if (r->ts)
		refer_free(r->ts);
}

static cparse_parse_s* cparse_inner_parse_ts_nstring(cparse_parse_string_s *restrict pri, cparse_inst_s *restrict inst, cparse_inst_context_t *restrict context)
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
		v = cparse_inner_alloc_value_nstring(pri->type, string, length);
		if (v)
		{
			if (vattr_insert_tail(context->scope, pri->key, v))
				ret = &pri->parse;
			refer_free(v);
		}
	}
	return ret;
}

cparse_parse_s* cparse_inner_alloc_parse_string(void)
{
	cparse_parse_string_s *restrict r;
	if ((r = (cparse_parse_string_s *) refer_alloz(sizeof(cparse_parse_string_s))))
	{
		refer_set_free(r, (refer_free_f) cparse_parse_string_free_func);
		r->parse.parse = (cparse_parse_f) cparse_inner_parse_ts_nstring;
		if ((r->ts = tparse_tstring_alloc_c_parse_multi_quotes()))
		{
			r->key = "\"";
			r->type = cparse_value$string;
			return &r->parse;
		}
		refer_free(r);
	}
	return NULL;
}

cparse_parse_s* cparse_inner_alloc_parse_chars(void)
{
	cparse_parse_string_s *restrict r;
	if ((r = (cparse_parse_string_s *) refer_alloz(sizeof(cparse_parse_string_s))))
	{
		refer_set_free(r, (refer_free_f) cparse_parse_string_free_func);
		r->parse.parse = (cparse_parse_f) cparse_inner_parse_ts_nstring;
		if ((r->ts = tparse_tstring_alloc_c_parse_single_quotes()))
		{
			r->key = "\'";
			r->type = cparse_value$chars;
			return &r->parse;
		}
		refer_free(r);
	}
	return NULL;
}
