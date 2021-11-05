#include "cparse.type.h"

static void cparse_inst_free_func(cparse_inst_s *restrict r)
{
	if (r->stack)
		refer_free(r->stack);
	if (r->parse)
		refer_free(r->parse);
}

cparse_inst_s* cparse_inst_alloc(void)
{
	cparse_inst_s *restrict r;
	r = (cparse_inst_s *) refer_alloz(sizeof(cparse_inst_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) cparse_inst_free_func);
		if (
			(r->stack = tparse_tstack_alloc()) &&
			(r->parse = cparse_inner_alloc_tmapping_parse())
		) return r;
		refer_free(r);
	}
	return NULL;
}

void cparse_inst_clear(cparse_inst_s *restrict inst)
{
	tparse_tstack_clear(inst->stack);
	inst->parse->clear(inst->parse);
}
