#include "kiya.h"
#include "kiya_args.h"
#include <pocket/pocket-verify.h>
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
	hashmap_t args;       // =weak> (kiya_args_t *)
	hashmap_t tag;        // =weak> (kiya_tag_f)
	hashmap_t parse;      // =weak> (kiya_parse_f)
	hashmap_t pocket;     // =weak> (pocket_s *)
	hashmap_t source;     // =>     (const char *)
	hashmap_t dylink;     // =weak> (dylink_pool_t *)
	hashmap_t dylib;      // =>     dlopen()
	dylink_pool_t *pool;
	kiya_args_pool_t *ap;
	kiya_kirakira_t *kirakira;
	const pocket_verify_s *verify;
	mlog_s *mlog;
};

static kiya_t* kiya_load_core(kiya_t *restrict kiya);
kiya_t* kiya_alloc(mlog_s *restrict mlog, uintptr_t xsize, uintptr_t asize)
{
	kiya_t *restrict kiya;
	kiya = (kiya_t *) calloc(1, sizeof(kiya_t));
	if (kiya)
	{
		if (hashmap_init(&kiya->kiya) &&
			hashmap_init(&kiya->args) &&
			hashmap_init(&kiya->tag) &&
			hashmap_init(&kiya->parse) &&
			hashmap_init(&kiya->pocket) &&
			hashmap_init(&kiya->source) &&
			hashmap_init(&kiya->dylink) &&
			hashmap_init(&kiya->dylib))
		{
			kiya->mlog = (mlog_s *) refer_save(mlog);
			kiya->pool = dylink_pool_alloc(dylink_mechine_x86_64, m_x86_64_dylink_set, m_x86_64_dylink_plt_set, xsize);
			kiya->ap = kiya_args_pool_alloc(asize);
			if (kiya->pool && kiya->ap)
			{
				if (kiya_load_core(kiya))
					return kiya;
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
	if (kiya->ap) kiya_args_pool_free(kiya->ap);
	if (kiya->verify) refer_free(kiya->verify);
	if (kiya->mlog) refer_free(kiya->mlog);
	hashmap_uini(&kiya->dylib);
	hashmap_uini(&kiya->dylink);
	hashmap_uini(&kiya->source);
	hashmap_uini(&kiya->pocket);
	hashmap_uini(&kiya->parse);
	hashmap_uini(&kiya->tag);
	hashmap_uini(&kiya->args);
	hashmap_uini(&kiya->kiya);
	free(kiya);
}

void kiya_set_verify(kiya_t *restrict kiya, const struct pocket_verify_s *verify)
{
	if (kiya->verify)
		refer_free(kiya->verify);
	kiya->verify = refer_save(verify);
}

static void kiya_source_free_func(hashmap_vlist_t *restrict vl)
{
	if (vl->value)
		free(vl->value);
}

kiya_t* kiya_set_source(kiya_t *restrict kiya, const char *restrict name, const char *restrict path)
{
	hashmap_vlist_t *restrict vl;
	size_t n;
	if (name && path)
	{
		vl = hashmap_put_name(&kiya->source, name, NULL, kiya_source_free_func);
		if (vl && !vl->value)
		{
			n = strlen(path) + 1;
			if ((vl->value = malloc(n)))
			{
				memcpy(vl->value, path, n);
				return kiya;
			}
			hashmap_delete_name(&kiya->source, name);
		}
	}
	return NULL;
}

static void kiya_args_free_func(hashmap_vlist_t *restrict vl);
kiya_t* kiya_set_arg(kiya_t *restrict kiya, const char *restrict name, const char *restrict value)
{
	kiya_args_t *restrict args;
	if (name && value)
	{
		if ((args = hashmap_get_name(&kiya->args, name))) goto go;
		else if ((args = kiya_args_alloc()))
		{
			register hashmap_vlist_t *restrict vl;
			if ((vl =  hashmap_set_name(&kiya->args, name, args, kiya_args_free_func)))
			{
				kiya_args_idol(args, vl->name);
				go:
				if (kiya_args_set(kiya->ap, args, value))
					return kiya;
			}
			kiya_args_free(args);
		}
	}
	return NULL;
}

kiya_t* kiya_load(kiya_t *restrict kiya, pocket_s *restrict pocket)
{
	const char *restrict tag;
	kiya_tag_f parse;
	tag = pocket_header(pocket)->tag.string;
	if (tag && (parse = (kiya_tag_f) hashmap_get_name(&kiya->tag, tag)))
	{
		if (parse(pocket))
			return kiya;
	}
	else mlog_printf(kiya->mlog, "pocket unknow tag(%s)\n", tag);
	return NULL;
}

static kiya_t* kiya_kirakira_check_like(kiya_t *restrict kiya, const char *restrict name);
kiya_t* kiya_load_path(kiya_t *restrict kiya, const char *restrict path)
{
	pocket_s *restrict pocket;
	pocket = pocket_load(path, kiya->verify);
	if (pocket)
	{
		kiya = kiya_load(kiya, pocket);
		refer_free(pocket);
		label_okay:
		return kiya;
	}
	else if (kiya_kirakira_check_like(kiya, path))
		goto label_okay;
	else mlog_printf(kiya->mlog, "kiya pocket(%s) load fail\n", path);
	return NULL;
}

void* kiya_symbol(kiya_t *restrict kiya, const char *name, const char *restrict symbol)
{
	dylink_pool_t *pool;
	if (name) pool = hashmap_get_name(&kiya->dylink, name);
	else pool = kiya->pool;
	if (pool && symbol)
		return dylink_pool_get_symbol(pool, symbol, NULL);
	return NULL;
}

kiya_t* kiya_do(kiya_t *restrict kiya, const char *name, const char *restrict main, int *restrict ret)
{
	kiya_main_f func;
	const kiya_args_t *restrict args;
	int r;
	if (main && (func = (kiya_main_f) kiya_symbol(kiya, NULL, main)))
	{
		if (!name) name = main;
		if ((args = (kiya_args_t *) hashmap_get_name(&kiya->args, name)))
			r = func(args->n, args->argv);
		else r = func(1, &name);
		if (ret) *ret = r;
		return kiya;
	}
	return NULL;
}

static kiya_t* kiya_kirakira_check_like(kiya_t *restrict kiya, const char *restrict name)
{
	hashmap_vlist_t *restrict vl;
	const char *restrict path;
	if (hashmap_get_name(&kiya->kiya, name))
	{
		label_okay:
		return kiya;
	}
	if ((vl = hashmap_find_name(&kiya->source, name)) && (path = (const char *) vl->value))
	{
		vl->value = NULL;
		if (!kiya_load_path(kiya, path))
			name = NULL;
		vl->value = (void *) path;
		if (name)
		{
			hashmap_delete_name(&kiya->source, name);
			if (hashmap_get_name(&kiya->kiya, name))
				goto label_okay;
		}
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
	// check [$ "kiya"]
	if (!(index_kiya = pocket_find_tag(pocket, (const pocket_attr_t *) header->system.ptr, "kiya", pocket_tag$index, NULL)))
		goto label_kiya;
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
			if (!kiya_kirakira_check_like(kiya, v->name.string))
				goto label_like;
			++v;
		}
	}
	// check [$ "kiya"] "name"
	if (!(v = pocket_find_tag(pocket, index_kiya, "name", pocket_tag$string, NULL)))
		goto label_name;
	if (!v->data.ptr) goto label_name;
	if (hashmap_find_name(&kiya->kiya, (const char *) v->data.ptr))
		goto label_conflict_name;
	return index_kiya;
	label_fail:
	return NULL;
	label_kiya:
	mlog_printf(kiya->mlog, "pocket miss [$ \"kiya\"]\n");
	goto label_fail;
	label_error_like:
	mlog_printf(kiya->mlog, "kiya [$ \"kiya\" \"like\"] error(name: %s, tag: %s)\n", v->name.string, v->tag.string);
	goto label_fail;
	label_like:
	mlog_printf(kiya->mlog, "kiya miss like(%s)\n", v->name.string);
	goto label_fail;
	label_name:
	mlog_printf(kiya->mlog, "pocket miss [$ \"kiya\"] \"name\" @string\n");
	goto label_fail;
	label_conflict_name:
	mlog_printf(kiya->mlog, "kiya conflict (%s)\n", (const char *) v->data.ptr);
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
		if ((vl->value = r = dlopen(name, RTLD_LOCAL | RTLD_NOW)))
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
	const char *restrict name;
	void *symbol;
	n = v->size;
	v = (const pocket_attr_t *) v->data.ptr;
	while (n)
	{
		--n;
		if (!v->name.string)
			goto label_fail;
		switch (pocket_preset_tag(pocket, v))
		{
			case pocket_tag$null:
				name = v->name.string;
				break;
			case pocket_tag$string:
				if (!(name = (const char *) v->data.ptr))
					goto label_fail;
				break;
			default:
				goto label_fail;
		}
		if (!(symbol = dlsym(dl, v->name.string))) goto label_fail;
		if (dylink_pool_set_func(pool, name, symbol))
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
		const kiya_args_t *restrict args;
		const char *error;
		if ((args = (kiya_args_t *) hashmap_get_name(&kiya->args, kira->name)))
			error = kira->initial(args->n, args->argv);
		else error = kira->initial(1, &kira->name);
		if (error)
		{
			mlog_printf(kiya->mlog, "kiya initial fail: %s\n", error);
			return NULL;
		}
	}
	return kira;
}

static pocket_s* kiya_load_kiya(pocket_s *restrict pocket, kiya_t *restrict kiya)
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
			return pocket;
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

static pocket_s* kiya_load_kiiyaa(pocket_s *restrict pocket, kiya_t *restrict kiya)
{
	const pocket_attr_t *restrict v, *restrict root;
	pocket_s *restrict p;
	if ((v = pocket_system(pocket)))
		v = pocket_find_tag(pocket, v, "kiiyaa", pocket_tag$string, NULL);
	if ((root = pocket_user(pocket)) && v && v->data.ptr)
		root = pocket_find_tag(pocket, root, (const char *) v->data.ptr, pocket_tag$index, NULL);
	if (root)
	{
		uint64_t n;
		n = root->size;
		v = (const pocket_attr_t *) root->data.ptr;
		while (n)
		{
			--n;
			if (!v->tag.string || strcmp(v->tag.string, "kiya") || !v->size)
				goto label_tag;
			p = pocket_alloc((uint8_t *) v->data.ptr, v->size, NULL);
			if (!p) goto label_load;
			pocket_set_depend(p, pocket);
			if (!kiya_load(kiya, p))
				goto label_load;
			refer_free(p);
			++v;
		}
	}
	return pocket;
	label_fail:
	return NULL;
	label_tag:
	mlog_printf(kiya->mlog, "kiiyaa parse(%s) fail: tag(%s)\n", v->name.string, v->tag.string);
	goto label_fail;
	label_load:
	if (p) refer_free(p);
	mlog_printf(kiya->mlog, "kiiyaa load(%s) fail\n", v->name.string);
	goto label_fail;
}

static pocket_s* kiya_load_source(pocket_s *restrict pocket, kiya_t *restrict kiya)
{
	const pocket_attr_t *restrict v, *restrict root;
	if ((root = pocket_user(pocket)))
	{
		uint64_t n;
		n = root->size;
		v = (const pocket_attr_t *) root->data.ptr;
		while (n)
		{
			--n;
			if (!v->name.string)
				goto label_name;
			if (pocket_preset_tag(pocket, v) != pocket_tag$string || !v->data.ptr)
				goto label_tag;
			if (hashmap_get_name(&kiya->pocket, v->name.string))
				goto label_name;
			if (hashmap_get_name(&kiya->source, v->name.string))
				goto label_name;
			if (!kiya_set_source(kiya, v->name.string, (const char *) v->data.ptr))
				goto label_set;
			++v;
		}
	}
	return pocket;
	label_fail:
	return NULL;
	label_tag:
	mlog_printf(kiya->mlog, "source parse(%s) fail: tag(%s)\n", v->name.string, v->tag.string);
	goto label_fail;
	label_name:
	mlog_printf(kiya->mlog, "source name conflict (%s)\n", v->name.string);
	goto label_fail;
	label_set:
	mlog_printf(kiya->mlog, "source set fail: name(%s), path(%s)\n", v->name.string, v->data.ptr);
	goto label_fail;
}

static void kiya_args_free_func(hashmap_vlist_t *restrict vl)
{
	if (vl->value)
		kiya_args_free((kiya_args_t *) vl->value);
}

static int kiya_dylink_pool_report(mlog_s *mlog, dylink_pool_report_type_t type, const char *restrict symbol, void *ptr, void **plt)
{
	if (type == dylink_pool_report_type_import_fail)
		mlog_printf(mlog, "[dylink] import (%s) fail\n", symbol);
	else if (type == dylink_pool_report_type_export_fail)
		mlog_printf(mlog, "[dylink] export (%s) fail\n", symbol);
	return 0;
}

static kiya_t* kiya_load_core(kiya_t *restrict kiya)
{
	static const char *restrict s_kiya = "$kiya";
	static const char *restrict s_args = "$args";
	static const char *restrict s_tag = "$tag";
	static const char *restrict s_parse = "$parse";
	static const char *restrict s_pocket = "$pocket";
	static const char *restrict s_source = "$source";
	static const char *restrict s_dylink = "$dylink";
	static const char *restrict s_mlog = "$mlog";
	#include "core/core.h"
	dylink_pool_t *restrict pool;
	void *v;
	kiya_t *r;
	r = NULL;
	dylink_pool_set_report(kiya->pool, (dylink_pool_report_f) kiya_dylink_pool_report, kiya->mlog);
	pool = dylink_pool_alloc_local(kiya->pool);
	if (pool)
	{
		if (!dylink_pool_set_func(kiya->pool, "malloc", malloc) &&
			!dylink_pool_set_func(kiya->pool, "calloc", calloc) &&
			!dylink_pool_set_func(kiya->pool, "realloc", realloc) &&
			!dylink_pool_set_func(kiya->pool, "free", free) &&
			!dylink_pool_set_func(pool, "strcmp", strcmp) &&
			!dylink_pool_set_func(pool, "hashmap_get_name", hashmap_get_name) &&
			!dylink_pool_set_func(pool, "pocket_preset_tag", pocket_preset_tag) &&
			!dylink_pool_set_func(pool, "pocket_find", pocket_find) &&
			!dylink_pool_set_func(pool, "dylink_pool_sync_symbol", dylink_pool_sync_symbol) &&
			!dylink_pool_set_func(pool, "dylink_pool_sync_symbol_all", dylink_pool_sync_symbol_all) &&
			!dylink_pool_load(pool, dy_core, dy_core_size))
		{
			if (!(v = dylink_pool_get_symbol(pool, "kiya", NULL)))
				goto label_fail;
			*(kiya_t **) v = kiya;
			if (!(v = dylink_pool_get_symbol(pool, "kiya$load$kiya", NULL)))
				goto label_fail;
			*(void **) v = kiya_load_kiya;
			if (!(v = dylink_pool_get_symbol(pool, "kiya$load$kiiyaa", NULL)))
				goto label_fail;
			*(void **) v = kiya_load_kiiyaa;
			if (!(v = dylink_pool_get_symbol(pool, "kiya$load$source", NULL)))
				goto label_fail;
			*(void **) v = kiya_load_source;
			#define set_hashmap(k)  \
				if (!(v = dylink_pool_get_symbol(pool, s_##k, NULL))) goto label_fail;\
				*(hashmap_t **) v = &kiya->k;\
				if (dylink_pool_sync_symbol(pool, s_##k, s_##k)) goto label_fail
			#define set_pointer(k)  \
				if (!(v = dylink_pool_get_symbol(pool, s_##k, NULL))) goto label_fail;\
				*(void **) v = (void *) kiya->k;\
				if (dylink_pool_sync_symbol(pool, s_##k, s_##k)) goto label_fail
			set_hashmap(kiya);
			set_hashmap(args);
			set_hashmap(tag);
			set_hashmap(parse);
			set_hashmap(pocket);
			set_hashmap(source);
			set_hashmap(dylink);
			set_pointer(mlog);
			#undef set_hashmap
			#undef set_pointer
			if (!(v = dylink_pool_get_symbol(pool, "kiya$tag$kiya", NULL)))
				goto label_fail;
			if (!hashmap_set_name(&kiya->tag, "kiya", v, NULL))
				goto label_fail;
			if (!(v = dylink_pool_get_symbol(pool, "kiya$tag$kiiyaa", NULL)))
				goto label_fail;
			if (!hashmap_set_name(&kiya->tag, "kiiyaa", v, NULL))
				goto label_fail;
			if (!(v = dylink_pool_get_symbol(pool, "kiya$tag$source", NULL)))
				goto label_fail;
			if (!hashmap_set_name(&kiya->tag, "source", v, NULL))
				goto label_fail;
			if (!(v = dylink_pool_get_symbol(pool, "kiya$parse$export", NULL)))
				goto label_fail;
			if (!hashmap_set_name(&kiya->parse, "kiya.export", v, NULL))
				goto label_fail;
			r = kiya;
		}
		label_fail:
		dylink_pool_free(pool);
	}
	return r;
}
