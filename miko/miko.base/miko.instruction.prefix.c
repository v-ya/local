#include "miko.instruction.prefix.h"
#include "miko.iset.pool.h"
#include "miko.major.h"

static const miko_access_prefix_t* miko_instruction_prefix_get_count(const miko_access_prefix_t prefix[], uintptr_t *restrict count)
{
	uintptr_t i;
	if (prefix)
	{
		for (i = 0; prefix[i].action && prefix[i].major; ++i) ;
		*count = i;
		if (!prefix[i].segment && !prefix[i].action && !prefix[i].major && !prefix[i].minor)
			return prefix;
	}
	return NULL;
}

static void miko_instruction_prefix_free_func(miko_instruction_prefix_s *restrict r)
{
	uintptr_t i, n;
	n = r->count;
	for (i = 0; i < n; ++i)
	{
		refer_ck_free(r->prefix[i].segment);
		refer_ck_free(r->prefix[i].action);
		refer_ck_free(r->prefix[i].major);
		refer_ck_free(r->prefix[i].minor);
	}
}

miko_instruction_prefix_s* miko_instruction_prefix_alloc(const miko_iset_pool_s *restrict pool, const miko_access_prefix_t prefix[])
{
	miko_instruction_prefix_s *restrict r;
	const miko_major_s *restrict major;
	const miko_minor_s *restrict minor;
	uintptr_t count, i;
	if (pool && miko_instruction_prefix_get_count(prefix, &count) &&
		(r = (miko_instruction_prefix_s *) refer_alloz(
			sizeof(miko_instruction_prefix_s) +
			sizeof(miko_instruction_prefix_t) * count)))
	{
		refer_hook_free(r, instruction_prefix);
		r->count = count;
		for (i = 0; i < count; ++i)
		{
			major = NULL;
			minor = NULL;
			if (prefix[i].segment)
			{
				if (!(r->prefix[i].segment = miko_iset_pool_save_segment(pool, prefix[i].segment)))
					goto label_fail;
			}
			if (!(r->prefix[i].action = miko_iset_pool_save_action(pool, prefix[i].action)))
				goto label_fail;
			if (!(major = miko_iset_pool_save_major(pool, prefix[i].major)))
				goto label_fail;
			if (!(r->prefix[i].major = (refer_string_t) refer_save(major->name)))
				goto label_fail;
			if (prefix[i].minor)
			{
				if (!(minor = miko_major_save_minor(major, prefix[i].minor)))
					goto label_fail;
				if (!(r->prefix[i].minor = (refer_string_t) refer_save(minor->name)))
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
