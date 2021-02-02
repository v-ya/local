#include "pocket-verify.h"
#include <hashmap.h>

typedef struct pocket_verify_inst_s {
	pocket_verify_s interface;
	hashmap_t pool;
} pocket_verify_inst_s;

static void pocket_verify_inst_pool_free_func(hashmap_vlist_t *restrict vl)
{
	if (vl->value)
		refer_free(vl->value);
}

static void pocket_verify_inst_free_func(pocket_verify_inst_s *restrict r)
{
	hashmap_uini(&r->pool);
}

static const pocket_verify_entry_s* pocket_verify_method_func(const pocket_verify_inst_s *restrict pool, const char *restrict name)
{
	if (pool && name)
		return (pocket_verify_entry_s *) hashmap_get_name(&pool->pool, name);
	return NULL;
}

static pocket_verify_s* pocket_verify_set_func(pocket_verify_inst_s *restrict pool, const char *restrict name, const pocket_verify_entry_s *restrict entry)
{
	if (pool && name && entry && hashmap_set_name(&pool->pool, name, entry, pocket_verify_inst_pool_free_func))
	{
		refer_save(entry);
		return &pool->interface;
	}
	return NULL;
}

static void pocket_verify_unset_func(pocket_verify_inst_s *restrict pool, const char *restrict name)
{
	if (pool && name)
		hashmap_delete_name(&pool->pool, name);
}

static pocket_verify_inst_s* pocket_verify_inst_alloc(void)
{
	pocket_verify_inst_s *restrict r;
	r = (pocket_verify_inst_s *) refer_alloc(sizeof(pocket_verify_inst_s));
	if (r)
	{
		if (hashmap_init(&r->pool))
		{
			refer_set_free(r, (refer_free_f) pocket_verify_inst_free_func);
			r->interface.method = (pocket_verify_method_f) pocket_verify_method_func;
			r->interface.set = (pocket_verify_set_f) pocket_verify_set_func;
			r->interface.unset = (pocket_verify_unset_f) pocket_verify_unset_func;
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

static pocket_verify_inst_s* pocket_verify_inst_set_not_save(pocket_verify_inst_s *restrict inst, const char *restrict name, const pocket_verify_entry_s *restrict entry)
{
	if (entry && hashmap_set_name(&inst->pool, name, entry, pocket_verify_inst_pool_free_func))
		return inst;
	refer_free(entry);
	return NULL;
}

pocket_verify_entry_s* pocket_verify_entry_alloc(pocket_verify_build_f build, pocket_verify_check_f check, uint64_t size, pocket_tag_t tag, uint32_t align)
{
	pocket_verify_entry_s *restrict r;
	if ((r = (pocket_verify_entry_s *) refer_alloc(sizeof(pocket_verify_entry_s))))
	{
		r->build = build;
		r->check = check;
		r->size = size;
		r->tag = tag;
		r->align = align;
	}
	return r;
}

void pocket_verify_entry_init(pocket_verify_entry_s *restrict r, pocket_verify_build_f build, pocket_verify_check_f check, uint64_t size, pocket_tag_t tag, uint32_t align)
{
	r->build = build;
	r->check = check;
	r->size = size;
	r->tag = tag;
	r->align = align;
}

pocket_verify_s* pocket_verify_empty(void)
{
	return &pocket_verify_inst_alloc()->interface;
}

#include "verify/layer.h"

pocket_verify_s* pocket_verify_default(void)
{
	pocket_verify_inst_s *restrict r;
	if ((r = pocket_verify_inst_alloc()))
	{
		#define verify_set(name, version)  pocket_verify_inst_set_not_save(r, #name "." #version, pocket_verify$##name##$##version())
		#define verify_sete(nv, nf, version)  pocket_verify_inst_set_not_save(r, #nv "." #version, pocket_verify$##nf##$##version())
		if (
			// xor
			verify_set(xor, 1) &&
			verify_set(xor, 2) &&
			verify_set(xor, 4) &&
			verify_set(xor, 8) &&
			// rhash32
			verify_sete(*>^~32, rhash32, 4) &&
			verify_sete(*>^~32, rhash32, 8) &&
			verify_sete(*>^~32, rhash32, 16) &&
			verify_sete(*>^~32, rhash32, 32) &&
			// rhash64
			verify_sete(*>^~64, rhash64, 4) &&
			verify_sete(*>^~64, rhash64, 8) &&
			verify_sete(*>^~64, rhash64, 16) &&
			verify_sete(*>^~64, rhash64, 32) &&
		1) return &r->interface;
		#undef verify_set
		#undef verify_sete
		refer_free(r);
	}
	return NULL;
}
