#include "cparse.type.h"

typedef struct cparse_parse_key_s {
	cparse_parse_s parse;
	cparse_value_s *value;
	tparse_tword_edge_s *we;
	tparse_tstring_cache_s *cache;
} cparse_parse_key_s;

static void cparse_parse_string_free_func(cparse_parse_key_s *restrict r)
{
	if (r->value)
		refer_free(r->value);
	if (r->we)
		refer_free(r->we);
	if (r->cache)
		refer_free(r->cache);
}

static cparse_parse_s* cparse_inner_parse_key(cparse_parse_key_s *restrict pri, cparse_inst_s *restrict inst, cparse_inst_context_t *restrict context)
{
	const char *restrict key;
	uintptr_t pos;
	context->pos = tparse_tword_skip(pri->we, context->data, context->size, pos = context->pos - 1);
	if (context->pos > pos)
	{
		key = tparse_tstring_cache_get(pri->cache, context->data + pos, context->pos - pos);
		if (key && vattr_insert_tail(context->scope, key, pri->value))
			return &pri->parse;
	}
	return NULL;
}

cparse_parse_s* cparse_inner_alloc_parse_key(void)
{
	cparse_parse_key_s *restrict r;
	if ((r = (cparse_parse_key_s *) refer_alloz(sizeof(cparse_parse_key_s))))
	{
		refer_set_free(r, (refer_free_f) cparse_parse_string_free_func);
		r->parse.parse = (cparse_parse_f) cparse_inner_parse_key;
		if ((r->value = cparse_inner_alloc_value_empty(cparse_value$key)) &&
			(r->we = cparse_inner_alloc_tword_key()) &&
			(r->cache = tparse_tstring_cache_alloc()))
			return &r->parse;
		refer_free(r);
	}
	return NULL;
}
