#include "miko.instruction.xpos.h"
#include "miko.iset.pool.h"
#include "miko.major.h"

static const miko_xpos_prefix_t* miko_instruction_xpos_get_count(const miko_xpos_prefix_t xpos[], uintptr_t *restrict count)
{
	uintptr_t i;
	if (xpos)
	{
		for (i = 0; xpos[i].score && xpos[i].style && xpos[i].major; ++i) ;
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
	const miko_major_s *restrict major;
	const miko_minor_s *restrict minor;
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
			major = NULL;
			minor = NULL;
			if (!(r->xpos[i].score = miko_iset_pool_save_score(pool, xpos[i].score)))
				goto label_fail;
			if (!(r->xpos[i].style = miko_iset_pool_save_style(pool, xpos[i].style)))
				goto label_fail;
			if (!(major = miko_iset_pool_save_major(pool, xpos[i].major)))
				goto label_fail;
			if (!(r->xpos[i].major = (refer_string_t) refer_save(major->name)))
				goto label_fail;
			if (xpos[i].minor)
			{
				if (!(minor = miko_major_save_minor(major, xpos[i].minor)))
					goto label_fail;
				if (!(r->xpos[i].minor = (refer_string_t) refer_save(minor->name)))
					goto label_fail;
				refer_free(major);
			}
			refer_free(minor);
		}
		return r;
		label_fail:
		refer_ck_free(major);
		refer_ck_free(minor);
		refer_free(r);
	}
	return NULL;
}
