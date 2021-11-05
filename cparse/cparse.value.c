#include "cparse.type.h"

static void cparse_value_free_func(cparse_value_s *restrict r)
{
	if (r->data.refer)
		refer_free(r->data.refer);
}

struct cparse_value_s* cparse_inner_alloc_value_empty(cparse_value_type_t type)
{
	struct cparse_value_s *restrict r;
	if ((r = (struct cparse_value_s *) refer_alloc(sizeof(struct cparse_value_s))))
	{
		r->type = type;
		r->data.refer = NULL;
		refer_set_free(r, (refer_free_f) cparse_value_free_func);
	}
	return r;
}

struct cparse_value_s* cparse_inner_alloc_value_string(cparse_value_type_t type, const char *restrict string, uintptr_t length)
{
	struct cparse_value_s *restrict r;
	if ((r = cparse_inner_alloc_value_empty(type)))
	{
		if ((r->data.refer = refer_dump_string_with_length(string, length)))
			return r;
		refer_free(r);
	}
	return NULL;
}

struct cparse_value_s* cparse_inner_alloc_value_nstring(cparse_value_type_t type, const char *restrict string, uintptr_t length)
{
	struct cparse_value_s *restrict r;
	if ((r = cparse_inner_alloc_value_empty(type)))
	{
		if ((r->data.refer = refer_dump_nstring_with_length(string, length)))
			return r;
		refer_free(r);
	}
	return NULL;
}
