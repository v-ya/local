#include "cparse.type.h"

cparse_inst_s* cparse_inner_scope_parse(cparse_inst_s *restrict inst, cparse_scope_s *restrict scope)
{
	cparse_inst_context_t context;
	tparse_tmapping_s *restrict mp;
	cparse_parse_s *restrict p;
	const char *data;
	uintptr_t size;
	context.scope = scope->scope;
	context.code = scope->code;
	context.data = data = scope->code->string + scope->code_offset;
	context.size = size = scope->code_length;
	context.pos = 0;
	mp = inst->parse;
	while (context.pos < size)
	{
		if ((p = (cparse_parse_s *) mp->test(mp, data[context.pos++])))
		{
			if (!p->parse(p, inst, &context))
				goto label_fail;
		}
	}
	return inst;
	label_fail:
	return NULL;
}
