#include "adora.h"

static void abc_adora_ilink_free_func(abc_adora_ilink_s *restrict r)
{
	if (r->instr_name) refer_free(r->instr_name);
}

abc_adora_ilink_s* abc_adora_ilink_alloc(refer_nstring_t instr_name, uint64_t exist_iset_flags)
{
	abc_adora_ilink_s *restrict r;
	if (instr_name && (r = (abc_adora_ilink_s *) refer_alloz(sizeof(abc_adora_ilink_s))))
	{
		refer_set_free(r, (refer_free_f) abc_adora_ilink_free_func);
		r->exist_iset_flags = exist_iset_flags;
		r->instr_name = (refer_nstring_t) refer_save(instr_name);
		return r;
	}
	return NULL;
}
