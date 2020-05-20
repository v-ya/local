#include "phoneme.h"
#include <string.h>

static void phoneme_pool_free_func(phoneme_pool_s *restrict pp)
{
	if (pp->dypool) dylink_pool_free(pp->dypool);
	if (pp->var) json_free(pp->var);
	hashmap_uini(&pp->depend);
	hashmap_uini(&pp->name);
	hashmap_uini(&pp->envelope);
	hashmap_uini(&pp->basefre);
	hashmap_uini(&pp->details);
	hashmap_uini(&pp->arg2pri);
	hashmap_uini(&pp->arg);
	hashmap_uini(&pp->phoneme);
}

static int phoneme_pool_dypool_report(phoneme_pool_s *restrict pp, dylink_pool_report_type_t type, const char *symbol, void *ptr, void **plt)
{
	return 0;
}

phoneme_pool_s* phoneme_pool_alloc(dylink_pool_t *dypool)
{
	phoneme_pool_s *pp;
	if (dypool)
	{
		pp = refer_alloz(sizeof(phoneme_pool_s));
		if (pp)
		{
			refer_set_free(pp, (refer_free_f) phoneme_pool_free_func);
			if (!hashmap_init(&pp->depend)) goto Err;
			if (!hashmap_init(&pp->name)) goto Err;
			if (!hashmap_init(&pp->envelope)) goto Err;
			if (!hashmap_init(&pp->basefre)) goto Err;
			if (!hashmap_init(&pp->details)) goto Err;
			if (!hashmap_init(&pp->arg2pri)) goto Err;
			if (!hashmap_init(&pp->arg)) goto Err;
			if (!hashmap_init(&pp->phoneme)) goto Err;
			pp->dypool = dypool;
			dylink_pool_set_report(pp->dypool, (dylink_pool_report_f) phoneme_pool_dypool_report, pp);
			return pp;
			Err:
			refer_free(pp);
			pp = NULL;
		}
	}
	return NULL;
}

int phoneme_pool_test_package(phoneme_pool_s *restrict pp, const char *restrict package)
{
	return -!hashmap_find_name(&pp->depend, package);
}

int phoneme_pool_load_package(phoneme_pool_s *restrict pp, const char *restrict package, const char *restrict path)
{
	package = (const char *) hashmap_find_name(&pp->depend, package);
	if (package) return -!!strcmp(package, path);
	else return dylink_pool_load_file(pp->dypool, path);
}
