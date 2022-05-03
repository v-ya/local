#include "std.h"
#include "../vkaa.syntax.h"
#include "../vkaa.tpool.h"
#include "../vkaa.scope.h"
#include "../vkaa.execute.h"
#include "../vkaa.parse.h"

static void vkaa_std_free_func(vkaa_std_s *restrict r)
{
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
			(r->oplevel = vkaa_std_create_oplevel()) &&
			(r->tpool = vkaa_std_create_tpool(&r->typeid)) &&
			(r->parse = vkaa_std_create_parse(r->oplevel, &r->typeid)) &&
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
	if (r->std) refer_free(r->std);
}

vkaa_std_context_s* vkaa_std_context_alloc(const vkaa_std_s *restrict std)
{
	vkaa_std_context_s *restrict r;
	if (std && (r = (vkaa_std_context_s *) refer_alloz(sizeof(vkaa_std_context_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_std_context_free_func);
		r->std = (const vkaa_std_s *) refer_save(std);
		if ((r->exec = vkaa_execute_alloc()) &&
			(r->var = vkaa_tpool_var_create_by_id(std->tpool, std->typeid.id_scope, NULL)) &&
			(r->scope = (vkaa_scope_s *) refer_save(((vkaa_std_var_scope_s *) r->var)->scope)))
			return r;
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
	if (vkaa_parse_parse(&c, syntax->syntax_array, syntax->syntax_number))
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

vkaa_std_context_s* vkaa_std_context_exec(vkaa_std_context_s *restrict context, vkaa_var_s **restrict prvar)
{
	vkaa_var_s *restrict var;
	var = NULL;
	if (context->exec->execute_number)
	{
		if (!(var = vkaa_execute_do(context->exec)))
			context = NULL;
	}
	if (prvar) *prvar = var;
	return context;
}
