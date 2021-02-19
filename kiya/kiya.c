#include "kiya.h"
#include <dylink.h>
#include <hashmap.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>

typedef struct kiya_kirakira_t kiya_kirakira_t;
struct kiya_kirakira_t {
	kiya_kirakira_t *next;
	const char *name;
	pocket_s *pocket;
	dylink_pool_t *pool;
	kiya_initial_f initial;
	kiya_finally_f finally;
};

struct kiya_t {
	hashmap_t kiya;       // =weak> (kiya_kirakira_t *)
	hashmap_t args;       // =weak> ?
	hashmap_t parse;      // =weak> (kiya_parse_f)
	hashmap_t pocket;     // =weak> (pocket_s *)
	hashmap_t dylink;     // =weak> (dylink_pool_t *)
	hashmap_t dylib;      // => dlopen()
	dylink_pool_t *pool;
	kiya_kirakira_t *kirakira;
	mlog_s *mlog;
};

static kiya_t* kiya_load_core(kiya_t *restrict kiya);
kiya_t* kiya_alloc(mlog_s *restrict mlog, size_t xsize)
{
	kiya_t *restrict kiya;
	kiya = (kiya_t *) calloc(1, sizeof(kiya_t));
	if (kiya)
	{
		if (hashmap_init(&kiya->kiya) &&
			hashmap_init(&kiya->args) &&
			hashmap_init(&kiya->parse) &&
			hashmap_init(&kiya->pocket) &&
			hashmap_init(&kiya->dylink) &&
			hashmap_init(&kiya->dylib))
		{
			kiya->pool = dylink_pool_alloc(dylink_mechine_x86_64, m_x86_64_dylink_set, m_x86_64_dylink_plt_set, xsize);
			if (kiya->pool)
			{
				if (kiya_load_core(kiya))
				{
					kiya->mlog = (mlog_s *) refer_save(mlog);
					return kiya;
				}
			}
		}
		kiya_free(kiya);
	}
	return NULL;
}

void kiya_free(register kiya_t *restrict kiya)
{
	register kiya_kirakira_t *kira;
	while ((kira = kiya->kirakira))
	{
		kiya->kirakira = kira->next;
		if (kira->finally) kira->finally();
		hashmap_delete_name(&kiya->dylink, kira->name);
		hashmap_delete_name(&kiya->pocket, kira->name);
		hashmap_delete_name(&kiya->kiya, kira->name);
		dylink_pool_free(kira->pool);
		refer_free(kira->pocket);
		free(kira);
	}
	if (kiya->pool) dylink_pool_free(kiya->pool);
	if (kiya->mlog) refer_free(kiya->mlog);
	hashmap_uini(&kiya->dylib);
	hashmap_uini(&kiya->dylink);
	hashmap_uini(&kiya->pocket);
	hashmap_uini(&kiya->parse);
	hashmap_uini(&kiya->args);
	hashmap_uini(&kiya->kiya);
	free(kiya);
}

static const pocket_attr_t* kiya_kirakira_check(kiya_t *restrict kiya, const pocket_s *restrict pocket);
static kiya_kirakira_t* kiya_kirakira_dylink(kiya_kirakira_t *restrict kira, kiya_t *restrict kiya, const pocket_s *restrict pocket, const pocket_attr_t *restrict v);
static kiya_kirakira_t* kiya_kirakira_symbol(kiya_kirakira_t *restrict kira, const pocket_s *restrict pocket, const pocket_attr_t *restrict index_kiya, mlog_s *restrict mlog);
static kiya_t* kiya_kirakira_parse(kiya_t *restrict kiya, pocket_s *restrict pocket, const pocket_attr_t *restrict v, const char *restrict name);
static kiya_kirakira_t* kiya_kirakira_initial(kiya_kirakira_t *restrict kira, kiya_t *restrict kiya);

kiya_t* kiya_load(kiya_t *restrict kiya, pocket_s *restrict pocket)
{
	const pocket_attr_t *restrict index_kiya;
	kiya_kirakira_t *restrict kira;
	if ((index_kiya = kiya_kirakira_check(kiya, pocket)) &&
		(kira = (kiya_kirakira_t *) calloc(1, sizeof(kiya_kirakira_t))))
	{
		kira->name = (const char *) pocket_find_tag(pocket, index_kiya, "name", pocket_tag$string, NULL)->data.ptr;
		kira->pocket = (pocket_s *) refer_save(pocket);
		if (kiya_kirakira_dylink(kira, kiya, pocket, pocket_find_tag(pocket, index_kiya, "dylink", pocket_tag$index, NULL)) &&
			kiya_kirakira_symbol(kira, pocket, index_kiya, kiya->mlog) &&
			hashmap_set_name(&kiya->kiya, kira->name, kira, NULL) &&
			hashmap_set_name(&kiya->pocket, kira->name, kira->pocket, NULL) &&
			hashmap_set_name(&kiya->dylink, kira->name, kira->pool, NULL) &&
			kiya_kirakira_parse(kiya, pocket, pocket_find_tag(pocket, index_kiya, "parse", pocket_tag$index, NULL), kira->name) &&
			kiya_kirakira_initial(kira, kiya))
		{
			kira->next = kiya->kirakira;
			kiya->kirakira = kira;
			return kiya;
		}
		hashmap_delete_name(&kiya->dylink, kira->name);
		hashmap_delete_name(&kiya->pocket, kira->name);
		hashmap_delete_name(&kiya->kiya, kira->name);
		if (kira->pool) dylink_pool_free(kira->pool);
		if (kira->pocket) refer_free(kira->pocket);
		free(kira);
	}
	return NULL;
}

kiya_t* kiya_do(kiya_t *restrict kiya, const char *name, const char *restrict main, int *restrict ret)
{
	dylink_pool_t *pool;
	kiya_main_f func;
	if (name) pool = hashmap_get_name(&kiya->dylink, name);
	else pool = kiya->pool;
	if (pool && main && (func = (kiya_main_f) dylink_pool_get_symbol(pool, main, NULL)))
	{
		// 暂时不处理 kiya->args
		if (!name) name = main;
		*ret = func(1, &name);
		return kiya;
	}
	return NULL;
}

static const pocket_attr_t* kiya_kirakira_check(kiya_t *restrict kiya, const pocket_s *restrict pocket)
{
	const pocket_header_t *restrict header;
	const pocket_attr_t *restrict index_kiya;
	const pocket_attr_t *restrict v;
	index_kiya = NULL;
	header = pocket_header(pocket);
	// check tag
	if (!header->tag.string || strcmp(header->tag.string, "kiya"))
		goto label_tag;
	// check [$ "kiya"]
	if (!(index_kiya = pocket_find_tag(pocket, (const pocket_attr_t *) header->system.ptr, header->tag.string, pocket_tag$index, NULL)))
		goto label_kiya;
	// check [$ "kiya"] "name"
	if (!(v = pocket_find_tag(pocket, index_kiya, "name", pocket_tag$string, NULL)))
		goto label_name;
	if (!v->data.ptr) goto label_name;
	if (hashmap_find_name(&kiya->kiya, (const char *) v->data.ptr))
		goto label_conflict_name;
	// check [$ "kiya" "like"]
	if ((v = pocket_find_tag(pocket, index_kiya, "like", pocket_tag$index, NULL)))
	{
		uint64_t n;
		n = v->size;
		v = (const pocket_attr_t *) v->data.ptr;
		while (n)
		{
			--n;
			if (!v->name.string || pocket_preset_tag(pocket, v) != pocket_tag$null)
				goto label_error_like;
			if (!hashmap_get_name(&kiya->kiya, v->name.string))
				goto label_like;
			++v;
		}
	}
	return index_kiya;
	label_fail:
	return NULL;
	label_tag:
	mlog_printf(kiya->mlog, "pocket tag(%s) not kiya\n", header->tag.string);
	goto label_fail;
	label_kiya:
	mlog_printf(kiya->mlog, "pocket miss [$ \"kiya\"]\n");
	goto label_fail;
	label_name:
	mlog_printf(kiya->mlog, "pocket miss [$ \"kiya\"] \"name\" @string\n");
	goto label_fail;
	label_conflict_name:
	mlog_printf(kiya->mlog, "kiya conflict (%s)\n", (const char *) v->data.ptr);
	goto label_fail;
	label_error_like:
	mlog_printf(kiya->mlog, "kiya [$ \"kiya\" \"like\"] error(name: %s, tag: %s)\n", v->name.string, v->tag.string);
	goto label_fail;
	label_like:
	mlog_printf(kiya->mlog, "kiya miss like(%s)\n", v->name.string);
	goto label_fail;
}

static void kiya_dylib_free_func(hashmap_vlist_t *restrict vl)
{
	if (vl->value)
		dlclose(vl->value);
}

static void* kiya_kirakira_dylib(kiya_t *restrict kiya, const char *restrict name)
{
	hashmap_vlist_t *restrict vl;
	void *r;
	r = hashmap_get_name(&kiya->dylib, name);
	if (r) goto label_ok;
	vl = hashmap_put_name(&kiya->dylib, name, NULL, kiya_dylib_free_func);
	if (vl)
	{
		if ((vl->value = r = dlopen(name, RTLD_LOCAL)))
		{
			label_ok:
			return r;
		}
		mlog_printf(kiya->mlog, "kiya load [%s] fail: %s\n", name, dlerror());
	}
	return NULL;
}

static dylink_pool_t* kiya_kirakira_dylib_import(dylink_pool_t *restrict pool, const pocket_s *restrict pocket, const pocket_attr_t *restrict v, void *dl, const char *restrict *restrict p_symbol)
{
	uint64_t n;
	void *symbol;
	n = v->size;
	v = (const pocket_attr_t *) v->data.ptr;
	while (n)
	{
		--n;
		if (!v->name.string || pocket_preset_tag(pocket, v) != pocket_tag$string || !v->data.ptr)
			goto label_fail;
		symbol = dlsym(dl, v->name.string);
		if (!symbol) goto label_fail;
		if (dylink_pool_set_func(pool, (const char *) v->data.ptr, symbol))
			goto label_fail;
		++v;
	}
	return pool;
	label_fail:
	*p_symbol = v->name.string;
	return NULL;
}

static kiya_kirakira_t* kiya_kirakira_dylink(kiya_kirakira_t *restrict kira, kiya_t *restrict kiya, const pocket_s *restrict pocket, const pocket_attr_t *restrict v)
{
	uint64_t n;
	pocket_tag_t tag;
	const char *symbol;
	symbol = NULL;
	if (!v) goto label_ok;
	if (!(kira->pool = dylink_pool_alloc_local(kiya->pool)))
		goto label_fail;
	n = v->size;
	v = (const pocket_attr_t *) v->data.ptr;
	while (n)
	{
		--n;
		tag = pocket_preset_tag(pocket, v);
		if (tag == pocket_tag$index)
		{
			// so
			void *h;
			if (!v->name.string || !(h = kiya_kirakira_dylib(kiya, v->name.string)))
				goto label_dylink;
			if (!kiya_kirakira_dylib_import(kira->pool, pocket, v, h, &symbol))
				goto label_import;
		}
		else if (tag == pocket_tag$custom && !strcmp(v->tag.string, "dylink"))
		{
			// dylink
			if (!v->data.ptr || dylink_pool_load(kira->pool, (const uint8_t *) v->data.ptr, (size_t) v->size))
				goto label_dylink;
		}
		else goto label_tag;
		++v;
	}
	label_ok:
	return kira;
	label_fail:
	return NULL;
	label_tag:
	mlog_printf(kiya->mlog, "kiya [$ \"kiya\" \"dylink\"] \"%s\" miss tag(%s)\n", v->name.string, v->tag.string);
	goto label_fail;
	label_dylink:
	mlog_printf(kiya->mlog, "kiya [$ \"kiya\" \"dylink\"] \"%s\" load fail\n", v->name.string);
	goto label_fail;
	label_import:
	mlog_printf(kiya->mlog, "kiya [$ \"kiya\" \"dylink\"] \"%s\" import symbol(%s) fail\n", v->name.string, symbol);
	goto label_fail;
}

static kiya_kirakira_t* kiya_kirakira_symbol(kiya_kirakira_t *restrict kira, const pocket_s *restrict pocket, const pocket_attr_t *restrict index_kiya, mlog_s *restrict mlog)
{
	const pocket_attr_t *restrict symbol;
	if ((symbol = pocket_find_tag(pocket, index_kiya, "initial", pocket_tag$string, NULL)))
	{
		if (!symbol->data.ptr || !kira->pool ||
			!(kira->initial = (kiya_initial_f) dylink_pool_get_symbol(kira->pool, (const char *) symbol->data.ptr, NULL)))
			goto label_fail;
	}
	if ((symbol = pocket_find_tag(pocket, index_kiya, "finally", pocket_tag$string, NULL)))
	{
		if (!symbol->data.ptr || !kira->pool ||
			!(kira->finally = (kiya_finally_f) dylink_pool_get_symbol(kira->pool, (const char *) symbol->data.ptr, NULL)))
			goto label_fail;
	}
	return kira;
	label_fail:
	mlog_printf(mlog, "kiya [$ \"kiya\"] \"%s\" not find symbol(%s)\n", symbol->name.string, (const char *) symbol->data.ptr);
	return NULL;
}

static kiya_t* kiya_kirakira_parse(kiya_t *restrict kiya, pocket_s *restrict pocket, const pocket_attr_t *restrict v, const char *restrict name)
{
	const pocket_attr_t *restrict user, *restrict root;
	kiya_parse_f parse;
	const char *restrict error;
	uint64_t n;
	if (!v) goto label_ok;
	n = v->size;
	v = (const pocket_attr_t *) v->data.ptr;
	user = pocket_user(pocket);
	if (user && pocket_preset_tag(pocket, user) != pocket_tag$index)
		user = NULL;
	while (n)
	{
		--n;
		if (!v->name.string) goto label_parse;
		switch (pocket_preset_tag(pocket, v))
		{
			case pocket_tag$null:
				root = user;
				break;
			case pocket_tag$string:
				if (v->data.ptr)
				{
					root = pocket_find_tag(pocket, user, (const char *) v->data.ptr, pocket_tag$index, NULL);
					break;
				}
				// fall through
			default:
				goto label_parse;
		}
		if (!root || !(parse = (kiya_parse_f) hashmap_get_name(&kiya->parse, v->name.string)))
			goto label_parse;
		error = NULL;
		if (!parse(pocket, root, name, &error))
			goto label_parse_do;
		++v;
	}
	label_ok:
	return kiya;
	label_fail:
	return NULL;
	label_parse:
	mlog_printf(kiya->mlog, "kiya [$ \"kiya\" \"parse\"] \"%s\" miss parse\n", v->name.string);
	goto label_fail;
	label_parse_do:
	mlog_printf(kiya->mlog, "kiya parse(%s) fail: %s\n", v->name.string, error);
	goto label_fail;
}

static kiya_kirakira_t* kiya_kirakira_initial(kiya_kirakira_t *restrict kira, kiya_t *restrict kiya)
{
	if (kira->initial)
	{
		const char *error;
		// 暂时不处理 kiya->args
		if ((error = kira->initial(1, &kira->name)))
		{
			mlog_printf(kiya->mlog, "kiya initial fail: %s\n", error);
			return NULL;
		}
	}
	return kira;
}

static kiya_t* kiya_load_core(kiya_t *restrict kiya)
{
	static const char *restrict s_kiya = "$kiya";
	static const char *restrict s_args = "$args";
	static const char *restrict s_parse = "$parse";
	static const char *restrict s_pocket = "$pocket";
	static const char *restrict s_dylink = "$dylink";
	#include "core/core.h"
	dylink_pool_t *restrict pool;
	void *v;
	kiya_t *r;
	r = NULL;
	pool = dylink_pool_alloc_local(kiya->pool);
	if (pool)
	{
		if (!dylink_pool_set_func(pool, "hashmap_get_name", hashmap_get_name) &&
			!dylink_pool_set_func(pool, "pocket_find_tag", pocket_find_tag) &&
			!dylink_pool_set_func(pool, "pocket_preset_tag", pocket_preset_tag) &&
			!dylink_pool_set_func(pool, "dylink_pool_sync_symbol", dylink_pool_sync_symbol) &&
			!dylink_pool_load(pool, dy_core, dy_core_size))
		{
			#define set_hashmap(k)  \
				if (!(v = dylink_pool_get_symbol(pool, s_##k, NULL))) goto label_fail;\
				*(hashmap_t **) v = &kiya->k;\
				if (dylink_pool_sync_symbol(pool, s_##k, s_##k)) goto label_fail
			set_hashmap(kiya);
			set_hashmap(args);
			set_hashmap(parse);
			set_hashmap(pocket);
			set_hashmap(dylink);
			#undef set_hashmap
			if (!(v = dylink_pool_get_symbol(pool, "kiya$parse$export", NULL))) goto label_fail;
			if (hashmap_set_name(&kiya->parse, "kiya.export", v, NULL))
				r = kiya;
		}
		label_fail:
		dylink_pool_free(pool);
	}
	return r;
}
