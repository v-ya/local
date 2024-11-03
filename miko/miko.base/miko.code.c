#include "miko.code.h"
#include "miko.instruction.h"
#include <memory.h>

// code impl

static void miko_code_impl_free_func(miko_code_impl_s *restrict r)
{
	const miko_instr_t *restrict p;
	const uint8_t *restrict instr;
	const uintptr_t *restrict ipos;
	uintptr_t i, n;
	instr = r->instr.data;
	ipos = (const uintptr_t *) r->ipos.data;
	n = r->code.instr_count;
	for (i = 0; i < n; ++i)
	{
		p = (const miko_instr_t *) (instr + ipos[i]);
		refer_ck_free(p->instr_data);
	}
	exbuffer_uini(&r->array);
	exbuffer_uini(&r->ipos);
	exbuffer_uini(&r->instr);
}

miko_code_impl_s* miko_code_alloc(void)
{
	miko_code_impl_s *restrict r;
	if ((r = (miko_code_impl_s *) refer_alloz(sizeof(miko_code_impl_s))))
	{
		refer_hook_free(r, code_impl);
		if (exbuffer_init(&r->instr, 0) &&
			exbuffer_init(&r->ipos, 0) &&
			exbuffer_init(&r->array, 0))
			return r;
		refer_free(r);
	}
	return NULL;
}

miko_code_impl_s* miko_code_push_instruction(miko_code_impl_s *restrict r, const miko_env_runtime_s *restrict runtime, miko_index_t instruction, const miko_access_t *restrict access)
{
	const miko_instruction_s *restrict instr;
	miko_instr_t *restrict p;
	uintptr_t n, used, size;
	if ((instr = miko_env_runtime_instruction_by_index(runtime, instruction)))
	{
		n = instr->prefix->count;
		used = r->instr.used;
		size = sizeof(miko_instr_t) + n * sizeof(miko_access_t);
		if (exbuffer_need(&r->instr, used + size))
		{
			p = (miko_instr_t *) (r->instr.data + used);
			p->instruction = instruction;
			p->instr_func = instr->func.call;
			p->instr_data = instr->data;
			if ((p->count = n))
				memcpy(p->argv, access, n * sizeof(miko_access_t));
			if (exbuffer_append(&r->ipos, &used, sizeof(used)))
			{
				refer_save(p->instr_data);
				r->instr.used = used + size;
				r->code.instr_count += 1;
				return r;
			}
		}
	}
	return NULL;
}

miko_code_impl_s* miko_code_push_code(miko_code_impl_s *restrict r, const miko_code_s *restrict code)
{
	const miko_instr_t *const *restrict instr;
	const miko_instr_t *restrict p;
	uintptr_t instr_used, i, n;
	if (code)
	{
		instr_used = r->instr.used;
		instr = code->instr_array;
		n = code->instr_count;
		for (i = 0; i < n; ++i)
		{
			p = instr[i];
			if (!exbuffer_append(&r->instr, p, sizeof(miko_instr_t) + p->count * sizeof(miko_access_t)))
				goto label_fail;
			if (!exbuffer_append(&r->ipos, &r->instr.used, sizeof(uintptr_t)))
				goto label_fail;
		}
		for (i = 0; i < n; ++i)
			refer_save(instr[i]->instr_data);
		r->code.instr_count += n;
		return r;
		label_fail:
		r->instr.used = instr_used;
		r->ipos.used = r->code.instr_count * sizeof(uintptr_t);
	}
	return NULL;
}

miko_code_impl_s* miko_code_pop(miko_code_impl_s *restrict r, miko_count_t count)
{
	const miko_instr_t *restrict p;
	const uint8_t *restrict instr;
	const uintptr_t *restrict ipos;
	uintptr_t i, n;
	if (count && count <= (n = r->code.instr_count))
	{
		count = n - count;
		instr = r->instr.data;
		ipos = (const uintptr_t *) r->ipos.data;
		for (i = count; i < n; ++i)
		{
			p = (const miko_instr_t *) (instr + ipos[i]);
			refer_ck_free(p->instr_data);
		}
		r->instr.used = ((const uintptr_t *) r->ipos.data)[count];
		r->ipos.used = count * sizeof(uintptr_t);
		r->code.instr_count = count;
		return r;
	}
	return NULL;
}

miko_instr_t* miko_code_get_by_index(const miko_code_impl_s *restrict r, miko_offset_t index)
{
	const uint8_t *restrict instr;
	const uintptr_t *restrict ipos;
	if (index < r->code.instr_count)
	{
		instr = r->instr.data;
		ipos = (const uintptr_t *) r->ipos.data;
		return (miko_instr_t *) (instr + ipos[index]);
	}
	return NULL;
}

miko_code_s* miko_code_okay(miko_code_impl_s *restrict r)
{
	const miko_instr_t **restrict p;
	const uint8_t *restrict instr;
	const uintptr_t *restrict ipos;
	uintptr_t i, n;
	if ((p = (const miko_instr_t **) exbuffer_need(&r->array, r->code.instr_count * sizeof(const miko_instr_t *))))
	{
		instr = r->instr.data;
		ipos = (const uintptr_t *) r->ipos.data;
		n = r->code.instr_count;
		for (i = 0; i < n; ++i)
			*p = (const miko_instr_t *) (instr + ipos[i]);
		r->code.instr_array = p;
		return &r->code;
	}
	return NULL;
}
