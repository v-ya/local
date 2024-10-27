#include "miko.instruction.args.h"
#include "miko.iset.pool.h"

static const miko_instruction_argv_t* miko_instruction_args_get_count(const miko_instruction_argv_t argv[], uintptr_t *restrict argc)
{
	uintptr_t i;
	if (argv)
	{
		for (i = 0; argv[i].xfind_type && argv[i].value_type; ++i) ;
		*argc = i;
		if (!argv[i].xfind_type && !argv[i].value_type)
			return argv;
	}
	return NULL;
}

static void miko_instruction_args_free_func(miko_instruction_args_s *restrict r)
{
	uintptr_t i, n;
	n = r->argc;
	for (i = 0; i < n; ++i)
	{
		refer_ck_free(r->argv[i].xfind_type);
		refer_ck_free(r->argv[i].value_type);
	}
}

miko_instruction_args_s* miko_instruction_args_alloc(const miko_iset_pool_s *restrict pool, const miko_instruction_argv_t argv[])
{
	miko_instruction_args_s *restrict r;
	uintptr_t argc, i;
	if (pool && miko_instruction_args_get_count(argv, &argc) &&
		(r = (miko_instruction_args_s *) refer_alloz(
			sizeof(miko_instruction_args_s) +
			sizeof(miko_instruction_args_t) * argc)))
	{
		refer_hook_free(r, instruction_args);
		r->argc = argc;
		for (i = 0; i < argc; ++i)
		{
			if (!(r->argv[i].xfind_type = miko_iset_pool_save_type(pool, argv[i].xfind_type)))
				goto label_fail;
			if (!(r->argv[i].value_type = miko_iset_pool_save_type(pool, argv[i].value_type)))
				goto label_fail;
		}
		return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}
