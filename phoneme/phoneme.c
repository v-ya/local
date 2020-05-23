#include "phoneme.h"
#include <stdio.h>
#include <stdlib.h>
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

static uint64_t phoneme_pool_dypool_type(const char *type, const char **name)
{
	register uint64_t r;
	register size_t n;
	r = n = 0;
	for (n = 0; n < 8 && *type && *type != '$'; ++n, ++type)
	{
		r = (r << 8) | *(uint8_t *) type;
	}
	if (*type == '$') ++type;
	else
	{
		type = NULL;
		r = 0;
	}
	*name = type;
	return r;
}

static phoneme_src_name_s phoneme_pool_dypool_src_name(const char *symbol, uint64_t *type)
{
	char *r;
	const char *name;
	size_t n1, n2;
	r = NULL;
	name = strchr(symbol, '$');
	if (name)
	{
		n1 = (size_t) (name - symbol);
		*type = phoneme_pool_dypool_type(name + 1, &name);
		if (name)
		{
			n2 = strlen(name) + 1;
			r = refer_alloc(n1 + n2 + 1);
			if (r)
			{
				memcpy(r, symbol, n1);
				r[n1] = '.';
				memcpy(r + n1 + 1, name, n2);
			}
		}
	}
	return r;
}

static void phoneme_pool_symbol_free_func(hashmap_vlist_t *vl)
{
	if (vl) refer_free(vl->value);
}

static int phoneme_pool_dypool_report(phoneme_pool_s *restrict pp, dylink_pool_report_type_t type, const char *symbol, void *ptr, void **plt)
{
	phoneme_src_name_s sname;
	uint64_t phoneme_type;
	hashmap_t *symlist;
	int r;
	r = 0;
	if (type == dylink_pool_report_type_export_ok || type == dylink_pool_report_type_delete_symbol)
	{
		sname = phoneme_pool_dypool_src_name(symbol, &phoneme_type);
		if (sname)
		{
			switch (phoneme_type)
			{
				case (((uint64_t) 'enve' << 32) | (uint64_t) 'lope'):
					symlist = &pp->envelope;
					break;
				case (((uint64_t) 'bas' << 32) | (uint64_t) 'efre'):
					symlist = &pp->basefre;
					break;
				case (((uint64_t) 'det' << 32) | (uint64_t) 'ails'):
					symlist = &pp->details;
					break;
				case (((uint64_t) 'arg' << 32) | (uint64_t) '2pri'):
					symlist = &pp->arg2pri;
					break;
				default:
					symlist = NULL;
					break;
			}
			if (symlist)
			{
				if (type == dylink_pool_report_type_export_ok)
				{
					if (hashmap_find_name(&pp->name, sname))
					{
						printf("phoneme symbol [%s] conflict\n", sname);
						r = -1;
						goto Err;
					}
					if (!hashmap_set_name(&pp->name, sname, sname, phoneme_pool_symbol_free_func))
					{
						printf("set phoneme name [%s] fail\n", sname);
						r = -1;
						goto Err;
					}
					refer_save((refer_t) sname);
					if (!hashmap_set_name(symlist, sname, ptr, NULL))
					{
						printf("set phoneme symbol [%s] fail\n", sname);
						r = -1;
					}
				}
				else
				{
					hashmap_delete_name(symlist, sname);
					hashmap_delete_name(&pp->name, sname);
				}
			}
			Err:
			refer_free((refer_t) sname);
		}
	}
	else if (type == dylink_pool_report_type_import_fail) printf("import [%s] fail\n", symbol);
	else if (type == dylink_pool_report_type_export_fail) printf("export [%s] fail\n", symbol);
	return r;
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

static void phoneme_pool_depend_free_func(hashmap_vlist_t *vl)
{
	if (vl) free(vl->value);
}

static const char* phoneme_pool_set_depend(phoneme_pool_s *restrict pp, const char *restrict package, const char *restrict path)
{
	char *r;
	size_t n;
	n = strlen(path) + 1;
	r = (char *) malloc(n);
	if (r)
	{
		memcpy(r, path, n);
		if (!hashmap_set_name(&pp->depend, package, r, phoneme_pool_depend_free_func))
		{
			free(r);
			r = NULL;
		}
	}
	return r;
}

int phoneme_pool_load_package(phoneme_pool_s *restrict pp, const char *restrict package, const char *restrict path)
{
	const char *op;
	op = (const char *) hashmap_get_name(&pp->depend, package);
	if (op) return -!!strcmp(op, path);
	else
	{
		if (phoneme_pool_set_depend(pp, package, path))
		{
			if (!dylink_pool_load_file(pp->dypool, path)) return 0;
			hashmap_delete_name(&pp->depend, package);
		}
		return -1;
	}
}
