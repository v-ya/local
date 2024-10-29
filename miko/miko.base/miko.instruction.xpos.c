#include "miko.instruction.xpos.h"
#include "miko.iset.pool.h"

static const miko_xpos_prefix_t* miko_instruction_xpos_get_count(const miko_xpos_prefix_t xpos[], uintptr_t *restrict count)
{
	uintptr_t i;
	if (xpos)
	{
		for (i = 0; xpos[i].score && xpos[i].style && xpos[i].major && xpos[i].minor; ++i) ;
		*count = i;
		if (!xpos[i].score && !xpos[i].style && !xpos[i].major && !xpos[i].minor)
			return xpos;
	}
	return NULL;
}

static void miko_instruction_xpos_free_func(miko_instruction_xpos_s *restrict r)
{
	uintptr_t i, n;
	n = r->count;
	for (i = 0; i < n; ++i)
	{
		refer_ck_free(r->xpos[i].score);
		refer_ck_free(r->xpos[i].style);
		refer_ck_free(r->xpos[i].major);
		refer_ck_free(r->xpos[i].minor);
	}
}

miko_instruction_xpos_s* miko_instruction_xpos_alloc(const miko_iset_pool_s *restrict pool, const miko_xpos_prefix_t xpos[])
{
	miko_instruction_xpos_s *restrict r;
	uintptr_t count, i;
	if (pool && miko_instruction_xpos_get_count(xpos, &count) &&
		(r = (miko_instruction_xpos_s *) refer_alloz(
			sizeof(miko_instruction_xpos_s) +
			sizeof(miko_instruction_xpos_t) * count)))
	{
		refer_hook_free(r, instruction_xpos);
		r->count = count;
		for (i = 0; i < count; ++i)
		{
			#define miko_ia_save(_field_)  \
				if (!(r->xpos[i]._field_ = miko_iset_pool_save_type(pool, xpos[i]._field_)))\
					goto label_fail
			miko_ia_save(score);
			miko_ia_save(style);
			miko_ia_save(major);
			miko_ia_save(minor);
			#undef miko_ia_save
		}
		return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}
