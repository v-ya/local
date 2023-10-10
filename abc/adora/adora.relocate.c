#include "adora.h"
#include <exbuffer.h>

struct abc_adora_relocate_s {
	exbuffer_t relocate_buffer;
	const abc_adora_relocate_t *relocate_array;
	uintptr_t relocate_count;
};

static void abc_adora_relocate_free_func(abc_adora_relocate_s *restrict r)
{
	exbuffer_uini(&r->relocate_buffer);
}

abc_adora_relocate_s* abc_adora_relocate_alloc(void)
{
	abc_adora_relocate_s *restrict r;
	if ((r = (abc_adora_relocate_s *) refer_alloz(sizeof(abc_adora_relocate_s))))
	{
		refer_set_free(r, (refer_free_f) abc_adora_relocate_free_func);
		if (exbuffer_init(&r->relocate_buffer, 0))
			return r;
		refer_free(r);
	}
	return NULL;
}

void abc_adora_relocate_clear(abc_adora_relocate_s *restrict r)
{
	r->relocate_count = 0;
	exbuffer_clear(&r->relocate_buffer);
}

const abc_adora_relocate_t* abc_adora_relocate_append(abc_adora_relocate_s *restrict r, uint32_t source_symbol, uint32_t relocate_type, int64_t relocate_addend, uint64_t target_offset)
{
	abc_adora_relocate_t *restrict p;
	if ((p = (abc_adora_relocate_t *) exbuffer_need(&r->relocate_buffer, (r->relocate_count + 1) * sizeof(*p))))
	{
		r->relocate_array = p;
		p += r->relocate_count;
		p->source_symbol = source_symbol;
		p->relocate_type = relocate_type;
		p->relocate_addend = relocate_addend;
		p->target_offset = target_offset;
		r->relocate_count += 1;
		return p;
	}
	return NULL;
}

const abc_adora_relocate_t* abc_adora_relocate_mapping(abc_adora_relocate_s *restrict r, uintptr_t *restrict relocate_count)
{
	if (relocate_count) *relocate_count = r->relocate_count;
	return r->relocate_array;
}
