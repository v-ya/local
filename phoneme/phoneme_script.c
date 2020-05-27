#include "phoneme_script.h"
#include <stdio.h>
#include <string.h>

static void phoneme_script_free_func(phoneme_script_s *restrict ps)
{
	hashmap_uini(&ps->package);
	if (ps->phoneme_pool) refer_free(ps->phoneme_pool);
	if (ps->core_path) refer_free((refer_t) ps->core_path);
	if (ps->package_path) refer_free((refer_t) ps->package_path);
}

phoneme_script_s* phoneme_script_alloc(size_t xmsize, phoneme_script_sysfunc_f sysfunc, void *pri)
{
	register phoneme_script_s *r;
	dylink_pool_t *dyp;
	dyp = dylink_pool_alloc(dylink_mechine_x86_64, m_x86_64_dylink_set, m_x86_64_dylink_plt_set, xmsize);
	if (dyp)
	{
		if (!sysfunc || !sysfunc(dyp, pri))
		{
			r = (phoneme_script_s *) refer_alloz(sizeof(phoneme_script_s));
			if (r)
			{
				if (hashmap_init(&r->package))
				{
					refer_set_free(r, (refer_free_f) phoneme_script_free_func);
					if ((r->phoneme_pool = phoneme_pool_alloc(dyp)))
					{
						dyp = NULL;
						r->base_time = 0.5;
						r->base_volume = 0.5;
						r->base_fre_line = 440;
						r->base_fre_step = 12;
						r->dmax = 32;
						return r;
					}
				}
				refer_free(r);
			}
		}
		if (dyp) dylink_pool_free(dyp);
	}
	return NULL;
}

phoneme_src_name_s phoneme_script_set_core_path(phoneme_script_s *restrict ps, const char *restrict path)
{
	phoneme_src_name_s r;
	r = NULL;
	if (path)
	{
		r = phoneme_src_name_dump(path);
		if (!r) goto Err;
	}
	if (ps->core_path) refer_free((refer_t) ps->core_path);
	ps->core_path = r;
	Err:
	return r;
}

phoneme_src_name_s phoneme_script_set_package_path(phoneme_script_s *restrict ps, const char *restrict path)
{
	phoneme_src_name_s r;
	r = NULL;
	if (path)
	{
		r = phoneme_src_name_dump(path);
		if (!r) goto Err;
	}
	if (ps->package_path) refer_free((refer_t) ps->package_path);
	ps->package_path = r;
	Err:
	return r;
}

phoneme_script_s* phoneme_script_load_core(phoneme_script_s *restrict ps, const char *restrict core)
{
	register const char *path;
	char buffer[4 << 10];
	path = core;
	if (ps->core_path)
	{
		snprintf(buffer, sizeof(buffer), "%s/%s", ps->core_path, core);
		path = buffer;
	}
	return phoneme_pool_load_package(ps->phoneme_pool, core, path)?NULL:ps;
}

phoneme_script_s* phoneme_script_load_package(phoneme_script_s *restrict ps, const char *restrict package)
{
	register phoneme_src_name_s path;
	register json_inode_t *restrict json;
	register hashmap_vlist_t *restrict vl;
	size_t n1, n2;
	json = NULL;
	vl = hashmap_find_name(&ps->package, package);
	if (!vl)
	{
		if (ps->package_path)
		{
			n1 = strlen(ps->package_path);
			n2 = strlen(package) + 1;
			path = refer_alloc(n1 + n2 + 1);
			if (path)
			{
				memcpy((void *) path, ps->package_path, n1);
				((char *) path)[n1++] = '/';
				memcpy((void *) (path + n1), package, n2);
			}
		}
		else path = phoneme_src_name_dump(package);
		if (!path) goto Err;
		vl = hashmap_set_name(&ps->package, package, NULL, phoneme_hashmap_free_refer_func);
		if (!vl) goto Err;
		json = json_load(path);
		if (!json) goto Err;
		if (!phoneme_script_load_package_json(ps, json)) goto Err;
		json_free(json);
		vl->value = (void *) path;
	}
	else if (!vl->value)
	{
		path = NULL;
		Err:
		if (path)
		{
			if (json) json_free(json);
			if (vl) hashmap_delete_name(&ps->package, package);
			refer_free((refer_t) path);
		}
		ps = NULL;
	}
	return ps;
}

static phoneme_script_s* phoneme_script_load_type(phoneme_script_s *restrict ps, json_inode_t *type)
{
	json_inode_t *name, *arg2pri, *refer_name;
	name = json_object_find(type, "name");
	if (!name || name->type != json_inode_string) goto Err;
	arg2pri = json_object_find(type, "arg2pri");
	if (arg2pri && arg2pri->type != json_inode_string) goto Err;
	refer_name = json_object_find(type, "refer");
	if (refer_name && refer_name->type != json_inode_string) goto Err;
	phoneme_pool_set_arg_pool(ps->phoneme_pool, name->value.string, arg2pri?arg2pri->value.string:NULL, refer_name?refer_name->value.string:NULL);
	return ps;
	Err:
	return NULL;
}

static void phoneme_script_load_package_set_var_func(hashmap_vlist_t *restrict vl, phoneme_pool_s *restrict pp)
{
	json_inode_t *v;
	if (vl->name && (v = (json_inode_t *) vl->value) && (v = json_copy(v)))
	{
		if (!phoneme_pool_set_var(pp, vl->name, v))
			json_free(v);
	}
}

static void phoneme_script_load_package_set_arg_by_type_func(hashmap_vlist_t *restrict vl, phoneme_pool_s *restrict pp, const char *restrict type)
{
	json_inode_t *v;
	if (vl->name && (v = (json_inode_t *) vl->value) && (v = json_copy(v)))
	{
		if (!phoneme_pool_set_arg_json(pp, type, vl->name, v))
			json_free(v);
	}
}

static void phoneme_script_load_package_set_arg_func(hashmap_vlist_t *restrict vl, phoneme_pool_s *restrict pp)
{
	json_inode_t *v;
	if (vl->name && (v = (json_inode_t *) vl->value) && v->type == json_inode_object)
	{
		hashmap_call_v2(&v->value.object, (hashmap_func_call_v2_t) phoneme_script_load_package_set_arg_by_type_func, pp, (void *) vl->name);
	}
}

static void phoneme_script_load_package_set_phoneme_arg_template_func(hashmap_vlist_t *restrict vl, json_inode_t *restrict *restrict s)
{
	json_inode_t *v;
	if (vl->name && (v = (json_inode_t *) vl->value) && (v = json_copy(v)))
	{
		if (!json_set(s, vl->name, v))
			json_free(v);
	}
}

static void phoneme_script_load_package_set_phoneme_func(hashmap_vlist_t *restrict vl, phoneme_script_s *restrict ps, json_inode_t *restrict package)
{
	json_inode_t *o, *v, *s;
	uint32_t sdmax, dmax;
	if (vl->name && (o = (json_inode_t *) vl->value))
	{
		s = NULL;
		sdmax = ps->sdmax;
		dmax = ps->dmax;
		// 引用模板
		if (o->type == json_inode_object)
		{
			s = json_object_find(o, "@refer");
			if (s)
			{
				if (s->type != json_inode_string) goto Err;
				if (!(s = json_find(package, s->value.string))) goto Err;
				if (!(s = json_copy(s))) goto Err;
				hashmap_call(&o->value.object, (hashmap_func_call_t) phoneme_script_load_package_set_phoneme_arg_template_func, &s);
				o = s;
			}
		}
		// 覆盖 sdmax dmax
		if (o->type == json_inode_object)
		{
			if ((v = json_object_find(o, "@sdmax")) && v->type == json_inode_integer && v->value.integer >= 0)
				sdmax = (uint32_t) v->value.integer;
			if ((v = json_object_find(o, "@dmax")) && v->type == json_inode_integer && v->value.integer > 0)
				dmax = (uint32_t) v->value.integer;
		}
		phoneme_pool_set_phoneme_json(ps->phoneme_pool, vl->name, o, sdmax, dmax);
		if (s) json_free(s);
	}
	Err:
	return ;
}

phoneme_script_s* phoneme_script_load_package_json(phoneme_script_s *restrict ps, json_inode_t *restrict package)
{
	json_inode_t *o, *v;
	uint32_t i, n;
	if (package->type == json_inode_object)
	{
		// load package
		o = json_object_find(package, "package");
		if (o)
		{
			if (o->type != json_inode_array)
				goto Err;
			for (i = 0, n = o->value.array.number; i < n; ++i)
			{
				v = json_array_find(o, i);
				if (!v || v->type != json_inode_string) goto Err;
				if (!phoneme_script_load_package(ps, v->value.string)) goto Err;
			}
		}
		// load core
		o = json_object_find(package, "core");
		if (o)
		{
			if (o->type != json_inode_array)
				goto Err;
			for (i = 0, n = o->value.array.number; i < n; ++i)
			{
				v = json_array_find(o, i);
				if (!v || v->type != json_inode_string) goto Err;
				if (!phoneme_script_load_core(ps, v->value.string)) goto Err;
			}
		}
		// load type
		o = json_object_find(package, "type");
		if (o)
		{
			if (o->type != json_inode_array)
				goto Err;
			for (i = 0, n = o->value.array.number; i < n; ++i)
			{
				v = json_array_find(o, i);
				if (!v || v->type != json_inode_object) goto Err;
				if (!phoneme_script_load_type(ps, v)) goto Err;
			}
		}
		// load var
		o = json_object_find(package, "var");
		if (o)
		{
			if (o->type != json_inode_object)
				goto Err;
			hashmap_call(&o->value.object, (hashmap_func_call_t) phoneme_script_load_package_set_var_func, ps->phoneme_pool);
		}
		// load arg
		o = json_object_find(package, "arg");
		if (o)
		{
			if (o->type != json_inode_object)
				goto Err;
			hashmap_call(&o->value.object, (hashmap_func_call_t) phoneme_script_load_package_set_arg_func, ps->phoneme_pool);
		}
		// load phoneme
		o = json_object_find(package, "phoneme");
		if (o)
		{
			if (o->type != json_inode_object)
				goto Err;
			hashmap_call_v2(&o->value.object, (hashmap_func_call_v2_t) phoneme_script_load_package_set_phoneme_func, ps, package);
		}
		return ps;
	}
	Err:
	return NULL;
}
