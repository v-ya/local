#include "adora.h"
#include <exbuffer.h>
#include <memory.h>

struct abc_adora_icode_s {
	exbuffer_t icode_buffer;
	exbuffer_t vlist_buffer;
	const abc_adora_icode_t *icode_array;
	uintptr_t icode_count;
	const abc_adora_var_t *vlist_array;
	uintptr_t vlist_count;
};

static void abc_adora_icode_free_func(abc_adora_icode_s *restrict r)
{
	exbuffer_uini(&r->icode_buffer);
	exbuffer_uini(&r->vlist_buffer);
}

abc_adora_icode_s* abc_adora_icode_alloc(void)
{
	abc_adora_icode_s *restrict r;
	if ((r = (abc_adora_icode_s *) refer_alloz(sizeof(abc_adora_icode_s))))
	{
		refer_set_free(r, (refer_free_f) abc_adora_icode_free_func);
		if (exbuffer_init(&r->icode_buffer, 0) && exbuffer_init(&r->vlist_buffer, 0))
			return r;
		refer_free(r);
	}
	return NULL;
}

void abc_adora_icode_clear(abc_adora_icode_s *restrict c)
{
	c->icode_count = 0;
	c->vlist_count = 0;
	exbuffer_clear(&c->icode_buffer);
	exbuffer_clear(&c->vlist_buffer);
}

const abc_adora_icode_t* abc_adora_icode_append(abc_adora_icode_s *restrict c, const abc_adora_var_t *restrict icode, const abc_adora_var_t *restrict param_array, uintptr_t param_count)
{
	abc_adora_icode_t *restrict p;
	abc_adora_var_t *restrict va;
	if (!param_count)
		goto label_skip_va;
	if (!(va = (abc_adora_var_t *) exbuffer_need(&c->vlist_buffer, (c->vlist_count + param_count) * sizeof(*va))))
		goto label_fail;
	c->vlist_array = va;
	memcpy(va + c->vlist_count, param_array, sizeof(*va) * param_count);
	label_skip_va:
	if ((p = (abc_adora_icode_t *) exbuffer_need(&c->icode_buffer, (c->icode_count + 1) * sizeof(*p))))
	{
		c->icode_array = p;
		p += c->icode_count;
		p->icode = *icode;
		p->var_offset = c->vlist_count;
		p->var_number = param_count;
		c->vlist_count += param_count;
		c->icode_count += 1;
		return p;
	}
	label_fail:
	return NULL;
}

const abc_adora_icode_t* abc_adora_icode_mapping(abc_adora_icode_s *restrict r, uintptr_t *restrict icode_count)
{
	if (icode_count) *icode_count = r->icode_count;
	return r->icode_array;
}

const abc_adora_var_t* abc_adora_icode_vlist(abc_adora_icode_s *restrict r, uintptr_t *restrict var_count)
{
	if (var_count) *var_count = r->vlist_count;
	return r->vlist_array;
}
