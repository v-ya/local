#include "adora.h"
#include <exbuffer.h>

struct abc_adora_ilink_s {
	exbuffer_t ilink_buffer;
	const abc_adora_ilink_t *ilink_array;
	uintptr_t ilink_count;
};

static void abc_adora_ilink_free_func(abc_adora_ilink_s *restrict r)
{
	const abc_adora_ilink_t *restrict p;
	uintptr_t i, n;
	p = r->ilink_array;
	n = r->ilink_count;
	for (i = 0; i < n; ++i)
	{
		if (p[i].instr_name)
			refer_free(p[i].instr_name);
	}
	exbuffer_uini(&r->ilink_buffer);
}

abc_adora_ilink_s* abc_adora_ilink_alloc(void)
{
	abc_adora_ilink_s *restrict r;
	if ((r = (abc_adora_ilink_s *) refer_alloz(sizeof(abc_adora_ilink_s))))
	{
		refer_set_free(r, (refer_free_f) abc_adora_ilink_free_func);
		if (exbuffer_init(&r->ilink_buffer, 0))
			return r;
		refer_free(r);
	}
	return NULL;
}

abc_adora_ilink_s* abc_adora_ilink_append(abc_adora_ilink_s *restrict r, refer_nstring_t instr_name, uint64_t exist_iset_flags)
{
	abc_adora_ilink_t *restrict p;
	if (instr_name && (p = (abc_adora_ilink_t *) exbuffer_need(&r->ilink_buffer, (r->ilink_count + 1) * sizeof(*p))))
	{
		r->ilink_array = p;
		p += r->ilink_count;
		p->exist_iset_flags = exist_iset_flags;
		p->instr_name = refer_save(instr_name);
		r->ilink_count += 1;
		return r;
	}
	return NULL;
}

const abc_adora_ilink_t* abc_adora_ilink_mapping(const abc_adora_ilink_s *restrict r, uintptr_t *restrict ilink_count)
{
	if (ilink_count) *ilink_count = r->ilink_count;
	return r->ilink_array;
}
