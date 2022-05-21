#include "std.h"

static void vkaa_std_free_func(vkaa_std_s *restrict r)
{
	if (r->typeid) refer_free(r->typeid);
	if (r->stack) refer_free(r->stack);
	if (r->parse) refer_free(r->parse);
	if (r->tpool) refer_free(r->tpool);
	if (r->oplevel) refer_free(r->oplevel);
	if (r->syntaxor) refer_free(r->syntaxor);
}

const vkaa_std_s* vkaa_std_alloc(void)
{
	vkaa_std_s *restrict r;
	if ((r = (vkaa_std_s *) refer_alloz(sizeof(vkaa_std_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_std_free_func);
		if ((r->syntaxor = vkaa_syntaxor_alloc()) &&
			vkaa_syntaxor_add_comment(r->syntaxor, "#", "\n") &&
			vkaa_syntaxor_add_comment(r->syntaxor, "##", "##") &&
			(r->oplevel = vkaa_std_create_oplevel()) &&
			(r->tpool = vkaa_std_create_tpool(&r->typeid)) &&
			(r->parse = vkaa_std_create_parse(r->oplevel, r->typeid)) &&
			(r->stack = tparse_tstack_alloc()))
			return r;
		refer_free(r);
	}
	return NULL;
}

static void vkaa_std_context_free_func(vkaa_std_context_s *restrict r)
{
	if (r->exec) refer_free(r->exec);
	if (r->scope) refer_free(r->scope);
	if (r->var) refer_free(r->var);
	if (r->vclear) refer_free(r->vclear);
	if (r->std) refer_free(r->std);
}

vkaa_std_context_s* vkaa_std_context_alloc(const vkaa_std_s *restrict std)
{
	vkaa_std_context_s *restrict r;
	if (std && (r = (vkaa_std_context_s *) refer_alloz(sizeof(vkaa_std_context_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_std_context_free_func);
		r->std = (const vkaa_std_s *) refer_save(std);
		if ((r->vclear = vkaa_vclear_alloc()) && (r->exec = vkaa_execute_alloc()) &&
			(r->var = vkaa_tpool_var_create_by_id(std->tpool, std->typeid->id_scope, NULL)) &&
			(r->scope = (vkaa_scope_s *) refer_save(((vkaa_std_var_scope_s *) r->var)->scope)))
		{
			vkaa_scope_set_vclear(r->scope, r->vclear);
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

vkaa_std_context_s* vkaa_std_context_append_syntax(vkaa_std_context_s *restrict context, const vkaa_syntax_s *restrict syntax)
{
	const vkaa_std_s *restrict std;
	vkaa_std_context_s *rr;
	vkaa_parse_context_t c;
	rr = NULL;
	std = context->std;
	c.parse = std->parse;
	c.stack = std->stack;
	c.execute = context->exec;
	c.tpool = std->tpool;
	c.scope = context->scope;
	c.this = context->var;
	tparse_tstack_clear(c.stack);
	if (vkaa_parse_parse(&c, syntax->syntax_array, syntax->syntax_number, NULL) &&
		vkaa_execute_okay(c.execute))
		rr = context;
	tparse_tstack_clear(c.stack);
	return rr;
}

vkaa_std_context_s* vkaa_std_context_append_source(vkaa_std_context_s *restrict context, const char *restrict source_data, uintptr_t source_length)
{
	const vkaa_syntax_s *restrict s;
	if ((s = vkaa_syntax_alloc(context->std->syntaxor, source_data, source_length)))
	{
		context = vkaa_std_context_append_syntax(context, s);
		refer_free(s);
		return context;
	}
	return NULL;
}

uintptr_t vkaa_std_context_exec(vkaa_std_context_s *restrict context, const volatile uintptr_t *running)
{
	return vkaa_execute_do(context->exec, running);
}
