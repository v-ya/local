#include "adora.h"
#include <memory.h>

static uintptr_t abc_adora_instr_check(const abc_adora_instr_t *restrict instr)
{
	if (!instr->ilength && !instr->iwriter && instr->const_length != instr->const_instr_length)
		goto label_fail;
	if (instr->ilength && (instr->const_length || instr->const_instr_length))
		goto label_fail;
	if (instr->iwriter && instr->const_instr_length)
		goto label_fail;
	return sizeof(abc_adora_instr_s) +
		sizeof(abc_adora_vtype_t) * instr->vtype_count +
		sizeof(uint8_t) * instr->const_instr_length;
	label_fail:
	return 0;
}

static void abc_adora_instr_free_func(abc_adora_instr_s *restrict r)
{
	if (r->instr_name) refer_free(r->instr_name);
	if (r->instr.instr_pri) refer_free(r->instr.instr_pri);
}

abc_adora_instr_s* abc_adora_instr_alloc(refer_nstring_t instr_name, uintptr_t instr_index, const abc_adora_instr_t *restrict instr)
{
	abc_adora_instr_s *restrict r;
	uint8_t *restrict p;
	uintptr_t size;
	if (instr_name && instr && (size = abc_adora_instr_check(instr)) >= sizeof(abc_adora_instr_s) &&
		(r = (abc_adora_instr_s *) refer_alloz(size)))
	{
		refer_set_free(r, (refer_free_f) abc_adora_instr_free_func);
		r->instr_name = (refer_nstring_t) refer_save(instr_name);
		r->instr_index = instr_index;
		p = (uint8_t *) (r + 1);
		if ((r->instr.vtype_count = instr->vtype_count))
		{
			r->instr.vtype_array = (const abc_adora_vtype_t *) memcpy(
				p, instr->vtype_array, size = sizeof(abc_adora_vtype_t) * instr->vtype_count);
			p += size;
		}
		r->instr.ilength = instr->ilength;
		r->instr.iwriter = instr->iwriter;
		r->instr.instr_pri = refer_save(instr->instr_pri);
		r->instr.const_length = instr->const_length;
		if ((r->instr.const_instr_length = instr->const_instr_length))
		{
			r->instr.const_instr_data = (const uint8_t *) memcpy(
				p, instr->const_instr_data, size = sizeof(uint8_t) * instr->const_instr_length);
			p += size;
		}
		return r;
	}
	return NULL;
}

const abc_adora_instr_s* abc_adora_instr_match(const abc_adora_instr_s *restrict instr, uintptr_t vtype_count, const abc_adora_vtype_t *restrict vtype_array)
{
	const abc_adora_vtype_t *restrict p;
	uintptr_t i, n;
	uint32_t vf;
	if ((n = instr->instr.vtype_count) == vtype_count)
	{
		p = instr->instr.vtype_array;
		for (i = 0; i < n; ++i)
		{
			if (p[i].vtype != vtype_array[i].vtype)
				goto label_fail;
			vf = p[i].vflag;
			if ((vtype_array[i].vflag & vf) != vf)
				goto label_fail;
		}
		return instr;
	}
	label_fail:
	return NULL;
}
