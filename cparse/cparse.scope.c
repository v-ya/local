#include "cparse.type.h"

static void cparse_scope_free_func(cparse_scope_s *restrict r)
{
	if (r->scope)
		refer_free(r->scope);
	if (r->code)
		refer_free(r->code);
	if (r->path)
		refer_free(r->path);
	if (r->name)
		refer_free(r->name);
}

cparse_scope_s* cparse_scope_alloc(cparse_inst_s *restrict inst, refer_nstring_t code, refer_string_t path, refer_string_t name)
{
	cparse_scope_s *restrict r;
	if (code)
	{
		r = (cparse_scope_s *) refer_alloz(sizeof(cparse_scope_s));
		if (r)
		{
			refer_set_free(r, (refer_free_f) cparse_scope_free_func);
			if ((r->scope = vattr_alloc()))
			{
				r->code = (refer_nstring_t) refer_save(code);
				r->path = (refer_string_t) refer_save(path);
				r->name = (refer_string_t) refer_save(name);
				if (cparse_inner_scope_parse(inst, r->scope, code->string, code->length))
					return r;
			}
			refer_free(r);
		}
	}
	return NULL;
}

cparse_scope_s* cparse_scope_alloc_by_code_path(cparse_inst_s *restrict inst, refer_nstring_t code, const char *restrict code_path)
{
	cparse_scope_s *restrict scope;
	refer_string_t path, name;
	const char *restrict p, *restrict pn;
	scope = NULL;
	path = name = NULL;
	pn = code_path;
	for (p = pn; *p; ++p)
	{
		if (*p == '/')
			pn = p + 1;
	}
	if (pn > code_path && !(path = refer_dump_string_with_length(code_path, (uintptr_t) pn - (uintptr_t) code_path)))
		goto label_fail;
	if (p > pn && !(name = refer_dump_string_with_length(pn, (uintptr_t) p - (uintptr_t) pn)))
		goto label_fail;
	scope = cparse_scope_alloc(inst, code, path, name);
	label_fail:
	if (path)
		refer_free(path);
	if (name)
		refer_free(name);
	return scope;
}

#include <fsys.h>

cparse_scope_s* cparse_scope_load(cparse_inst_s *restrict inst, const char *restrict code_path)
{
	cparse_scope_s *restrict scope;
	fsys_file_s *restrict file;
	refer_nstring_t code;
	scope = NULL;
	if ((file = fsys_file_alloc(code_path, fsys_file_flag_read)))
	{
		code = fsys_file_aread_all(file);
		if (code)
		{
			scope = cparse_scope_alloc_by_code_path(inst, code, code_path);
			refer_free(code);
		}
		refer_free(file);
	}
	return scope;
}
