#include "phoneme_arg.h"

static void phoneme_arg_pool_free_func(register phoneme_arg_pool_s *restrict pap)
{
	hashmap_uini(&pap->json);
	hashmap_uini(&pap->pri);
}

phoneme_arg_pool_s* phoneme_arg_pool_alloc(void)
{
	register phoneme_arg_pool_s *restrict r;
	r = (phoneme_arg_pool_s *) refer_alloz(sizeof(phoneme_arg_pool_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) phoneme_arg_pool_free_func);
		if (hashmap_init(&r->json) && hashmap_init(&r->pri))
			return r;
		refer_free(r);
	}
	return NULL;
}

phoneme_arg_s* phoneme_arg_pool_set(register phoneme_arg_pool_s *restrict pool, const char *restrict name, phoneme_arg_s *restrict arg)
{
	if (arg)
	{
		refer_t pri;
		if (pool->arg2pri && (pri = pool->arg2pri(*arg)))
		{
			if (hashmap_set_name(&pool->json, name, arg, phoneme_hashmap_free_refer_func))
			{
				refer_save(arg);
				if (hashmap_set_name(&pool->pri, name, pri, phoneme_hashmap_free_refer_func))
					return arg;
				else
				{
					refer_free(pri);
					hashmap_delete_name(&pool->json, name);
				}
			}
		}
	}
	return NULL;
}

phoneme_arg_s* phoneme_arg_pool_get_arg(register phoneme_arg_pool_s *restrict pool, const char *restrict name)
{
	return (phoneme_arg_s *) refer_save(hashmap_get_name(&pool->json, name));
}

refer_t phoneme_arg_pool_get_pri(register phoneme_arg_pool_s *restrict pool, const char *restrict name)
{
	return refer_save(hashmap_get_name(&pool->pri, name));
}
