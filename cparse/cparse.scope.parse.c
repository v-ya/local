#include "cparse.type.h"

cparse_inst_s* cparse_inner_scope_parse(cparse_inst_s *restrict inst, vattr_s *restrict scope, const char *restrict data, uintptr_t size)
{
	cparse_inst_context_t context;
	tparse_tmapping_s *restrict mp;
	cparse_parse_s *restrict p;
	context.scope = scope;
	context.data = data;
	context.size = size;
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
