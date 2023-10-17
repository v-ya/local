#include "adora.h"

static void abc_adora_iset_free_func(abc_adora_iset_s *restrict r)
{
	if (r->iset_name) refer_free(r->iset_name);
}

abc_adora_iset_s* abc_adora_iset_alloc(const char *restrict iset_name, uint64_t iset_flag)
{
	abc_adora_iset_s *restrict r;
	if (iset_name && (r = (abc_adora_iset_s *) refer_alloz(sizeof(abc_adora_iset_s))))
	{
		refer_set_free(r, (refer_free_f) abc_adora_iset_free_func);
		r->iset_flag = iset_flag;
		if ((r->iset_name = refer_dump_nstring(iset_name)))
			return r;
		refer_free(r);
	}
	return NULL;
}
