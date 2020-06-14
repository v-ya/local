#include "phoneme_script.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

static void phoneme_script_free_func(phoneme_script_s *restrict ps)
{
	hashmap_uini(&ps->package);
	hashmap_uini(&ps->script);
	if (ps->phoneme_pool) refer_free(ps->phoneme_pool);
	if (ps->mlog) refer_free(ps->mlog);
	if (ps->core_path) refer_free((refer_t) ps->core_path);
	if (ps->package_path) refer_free((refer_t) ps->package_path);
}

phoneme_script_s* phoneme_script_alloc(size_t xmsize, mlog_s *restrict mlog, phoneme_script_sysfunc_f sysfunc, void *pri)
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
				refer_set_free(r, (refer_free_f) phoneme_script_free_func);
				if (hashmap_init(&r->package) && hashmap_init(&r->script))
				{
					if ((r->phoneme_pool = phoneme_pool_alloc(dyp)))
					{
						dyp = NULL;
						phoneme_pool_set_mlog(r->phoneme_pool, r->mlog = refer_save(mlog));
						r->base_time = 0.4054;
						r->base_volume = 0.5;
						r->base_fre_line = 440;
						r->base_fre_step = 12;
						r->space_time = 1;
						r->dmax = 32;
						r->vstack_script_max = 16;
						r->vstack_phoneme_max = 16;
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
	if (phoneme_pool_load_package(ps->phoneme_pool, core, path))
	{
		mlog_printf(ps->mlog, "load core[%s](%s) fail ...\n", core, path);
		return NULL;
	}
	return ps;
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
		if (!json)
		{
			mlog_printf(ps->mlog, "load package-json[%s] fail ...\n", path);
			goto Err;
		}
		if (!phoneme_script_load_package_json(ps, json))
		{
			mlog_printf(ps->mlog, "parse package-json[%s] fail ...\n", path);
			goto Err;
		}
		json_free(json);
		vl->value = (void *) path;
	}
	else if (!vl->value)
	{
		mlog_printf(ps->mlog, "package[%s] loop load ...\n", package);
		path = NULL;
		Err:
		mlog_printf(ps->mlog, "load package[%s] fail ...\n", package);
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
		hashmap_call_v2(&v->value.object, (hashmap_func_call_v2_f) phoneme_script_load_package_set_arg_by_type_func, pp, (void *) vl->name);
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
				hashmap_call(&o->value.object, (hashmap_func_call_f) phoneme_script_load_package_set_phoneme_arg_template_func, &s);
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
			hashmap_call(&o->value.object, (hashmap_func_call_f) phoneme_script_load_package_set_var_func, ps->phoneme_pool);
		}
		// load arg
		o = json_object_find(package, "arg");
		if (o)
		{
			if (o->type != json_inode_object)
				goto Err;
			hashmap_call(&o->value.object, (hashmap_func_call_f) phoneme_script_load_package_set_arg_func, ps->phoneme_pool);
		}
		// load phoneme
		o = json_object_find(package, "phoneme");
		if (o)
		{
			if (o->type != json_inode_object)
				goto Err;
			hashmap_call_v2(&o->value.object, (hashmap_func_call_v2_f) phoneme_script_load_package_set_phoneme_func, ps, package);
		}
		return ps;
	}
	Err:
	return NULL;
}

phoneme_s* phoneme_script_get_phoneme(phoneme_script_s *restrict ps, register const char *restrict *restrict script)
{
	phoneme_s *r;
	char phname[phoneme_var_path_max];
	r = NULL;
	if (phoneme_read_string(phname, sizeof(phname), script, "([ \t\r\n,;:") && *phname)
	{
		if (**script == '[')
		{
			++*script;
			r = phoneme_pool_get_phoneme_modify(ps->phoneme_pool, phname, script, ps->sdmax, ps->dmax);
			if (**script == ']') ++*script;
			else
			{
				if (r)
				{
					refer_free(r);
					r = NULL;
				}
				mlog_printf(ps->mlog, "phoneme modify parse fail ...\n");
			}
		}
		else r = phoneme_pool_get_phoneme(ps->phoneme_pool, phname);
	}
	return r;
}


phoneme_output_s* phoneme_script_load(phoneme_script_s *restrict ps, const char *restrict script_path, phoneme_output_s *restrict po)
{
	char *script;
	FILE *fp;
	size_t size;
	script = NULL;
	if (hashmap_find_name(&ps->script, script_path))
		return NULL;
	fp = fopen(script_path, "r");
	if (fp)
	{
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		if (size && (script = (char *) malloc(size + 1)))
		{
			fseek(fp, 0, SEEK_SET);
			if (fread(script, 1, size, fp) != size)
			{
				free(script);
				script = NULL;
			}
		}
		fclose(fp);
	}
	if (script)
	{
		script[size] = 0;
		if (hashmap_set_name(&ps->script, script_path, NULL, NULL))
		{
			po = phoneme_script_run(ps, script, po);
			hashmap_delete_name(&ps->script, script_path);
		}
		else po = NULL;
		free(script);
	}
	else po = NULL;
	return po;
}

static int64_t phoneme_script_run_read_int(phoneme_script_s *restrict ps, register const char *restrict *restrict script)
{
	register json_inode_t *value;
	json_inode_t *need_free;
	int64_t r;
	value = phoneme_pool_read_value(ps->phoneme_pool, script, &need_free);
	if (value)
	{
		if (value->type == json_inode_integer)
			r = value->value.integer;
		else value = NULL;
		if (need_free) json_free(need_free);
		if (value) return r;
	}
	*script = NULL;
	return 0;
}

static double phoneme_script_run_read_double(phoneme_script_s *restrict ps, register const char *restrict *restrict script)
{
	register json_inode_t *value;
	json_inode_t *need_free;
	double r;
	value = phoneme_pool_read_value(ps->phoneme_pool, script, &need_free);
	if (value)
	{
		if (value->type == json_inode_floating)
			r = value->value.floating;
		else if (value->type == json_inode_integer)
			r = value->value.integer;
		else value = NULL;
		if (need_free) json_free(need_free);
		if (value) return r;
	}
	*script = NULL;
	return 0;
}

static const char* phoneme_script_run_conctrl_var(phoneme_script_s *restrict ps, const char *restrict script)
{
	json_inode_t *value, *need_free;
	const char *jpath;
	char path[phoneme_var_path_max];
	if (*script == '<')
	{
		++script;
		if (phoneme_read_string(path, sizeof(path), &script, ">") && *script == '>')
		{
			++script;
			while (phoneme_alpha_table_space[*(uint8_t*)script]) ++script;
			if (*script != '=') goto Err;
			++script;
			while (phoneme_alpha_table_space[*(uint8_t*)script]) ++script;
			value = phoneme_pool_read_value(ps->phoneme_pool, &script, &need_free);
			if (value && (need_free || (need_free = json_copy(value))))
			{
				jpath = NULL;
				if (*path) jpath = path;
				if (phoneme_pool_set_var(ps->phoneme_pool, jpath, need_free))
					return script;
				json_free(need_free);
			}
		}
	}
	Err:
	return NULL;
}

static const char* phoneme_script_run_conctrl_core(phoneme_script_s *restrict ps, const char *restrict script)
{
	json_inode_t *value, *need_free;
	value = phoneme_pool_read_value(ps->phoneme_pool, &script, &need_free);
	if (value)
	{
		if (value->type == json_inode_string)
		{
			if (!phoneme_script_set_core_path(ps, value->value.string))
				script = NULL;
		}
		else if (value->type == json_inode_null)
			phoneme_script_set_core_path(ps, NULL);
		else script = NULL;
		if (need_free) json_free(need_free);
	}
	else script = NULL;
	return script;
}

static const char* phoneme_script_run_conctrl_package(phoneme_script_s *restrict ps, const char *restrict script)
{
	json_inode_t *value, *need_free;
	value = phoneme_pool_read_value(ps->phoneme_pool, &script, &need_free);
	if (value)
	{
		if (value->type == json_inode_string)
		{
			if (!phoneme_script_set_package_path(ps, value->value.string))
				script = NULL;
		}
		else if (value->type == json_inode_null)
			phoneme_script_set_package_path(ps, NULL);
		else script = NULL;
		if (need_free) json_free(need_free);
	}
	else script = NULL;
	return script;
}

static const char* phoneme_script_run_conctrl_btime(phoneme_script_s *restrict ps, const char *restrict script)
{
	double n;
	n = phoneme_script_run_read_double(ps, &script);
	if (script && n > 0)
	{
		ps->base_time = n;
		return script;
	}
	return NULL;
}

static const char* phoneme_script_run_conctrl_volume(phoneme_script_s *restrict ps, const char *restrict script)
{
	double n;
	n = phoneme_script_run_read_double(ps, &script);
	if (script && n >= 0)
	{
		ps->base_volume = n;
		return script;
	}
	return NULL;
}

static const char* phoneme_script_run_conctrl_bfline(phoneme_script_s *restrict ps, const char *restrict script)
{
	double n;
	n = phoneme_script_run_read_double(ps, &script);
	if (script && n > 0)
	{
		ps->base_fre_line = n;
		return script;
	}
	return NULL;
}

static const char* phoneme_script_run_conctrl_bfstep(phoneme_script_s *restrict ps, const char *restrict script)
{
	double n;
	n = phoneme_script_run_read_double(ps, &script);
	if (script && n != 0)
	{
		ps->base_fre_step = n;
		return script;
	}
	return NULL;
}

static const char* phoneme_script_run_conctrl_dmax(phoneme_script_s *restrict ps, const char *restrict script)
{
	uint32_t n;
	n = (uint32_t) phoneme_script_run_read_int(ps, &script);
	if (script && n)
	{
		ps->dmax = n;
		return script;
	}
	return NULL;
}

static const char* phoneme_script_run_conctrl_sdmax(phoneme_script_s *restrict ps, const char *restrict script)
{
	uint32_t n;
	n = (uint32_t) phoneme_script_run_read_int(ps, &script);
	if (script)
	{
		ps->sdmax = n;
		return script;
	}
	return NULL;
}

static const char* phoneme_script_run_conctrl_import(phoneme_script_s *restrict ps, const char *restrict script)
{
	json_inode_t *value, *need_free;
	value = phoneme_pool_read_value(ps->phoneme_pool, &script, &need_free);
	if (value)
	{
		if (value->type == json_inode_string)
		{
			if (!phoneme_script_load_package(ps, value->value.string))
				script = NULL;
		}
		else script = NULL;
		if (need_free) json_free(need_free);
	}
	else script = NULL;
	return script;
}

static const char* phoneme_script_run_conctrl_phoneme(phoneme_script_s *restrict ps, const char *restrict script)
{
	phoneme_s *p;
	char phname[phoneme_var_path_max];
	if (phoneme_read_string(phname, sizeof(phname), &script, "=([ \t\r\n,;:") && *phname)
	{
		while (phoneme_alpha_table_space[*(uint8_t*)script]) ++script;
		if (*script == '=')
		{
			++script;
			while (phoneme_alpha_table_space[*(uint8_t*)script]) ++script;
			p = phoneme_script_get_phoneme(ps, &script);
			if (p)
			{
				if (phoneme_pool_set_phoneme(ps->phoneme_pool, phname, p))
				{
					refer_free(p);
					return script;
				}
				refer_free(p);
			}
		}
	}
	return NULL;
}

static const char* phoneme_script_run_conctrl_sampfre(phoneme_script_s *restrict ps, phoneme_output_s *restrict po, const char *restrict script)
{
	uint32_t n;
	n = (uint32_t) phoneme_script_run_read_int(ps, &script);
	if (script)
	{
		if (phoneme_output_set_sampfre(po, n))
			return script;
	}
	return NULL;
}

static const char* phoneme_script_run_conctrl_include(phoneme_script_s *restrict ps, phoneme_output_s *restrict po, const char *restrict script)
{
	json_inode_t *value, *need_free;
	value = phoneme_pool_read_value(ps->phoneme_pool, &script, &need_free);
	if (value)
	{
		if (value->type == json_inode_string)
		{
			if (!phoneme_script_load(ps, value->value.string, po))
				script = NULL;
		}
		else script = NULL;
		if (need_free) json_free(need_free);
	}
	else script = NULL;
	return script;
}

static const char* phoneme_script_run_conctrl(phoneme_script_s *restrict ps, const char *restrict script, phoneme_output_s *restrict po)
{
	char key[16];
	*(uint64_t *)key = 0;
	if (phoneme_read_string(key, sizeof(key), &script, " \t\r\n"))
	{
		while (phoneme_alpha_table_space[*(uint8_t*)script]) ++script;
		switch (*(uint64_t *)key)
		{
			case ((uint64_t) 'rav'):
				// var
				return phoneme_script_run_conctrl_var(ps, script);
			case ((uint64_t) 'eroc'):
				// core
				return phoneme_script_run_conctrl_core(ps, script);
			case ((uint64_t) 'xamd'):
				// dmax
				return phoneme_script_run_conctrl_dmax(ps, script);
			case (((uint64_t) 'e' << 32) | (uint64_t) 'mitb'):
				// btime
				return phoneme_script_run_conctrl_btime(ps, script);
			case (((uint64_t) 'x' << 32) | (uint64_t) 'amds'):
				// sdmax
				return phoneme_script_run_conctrl_sdmax(ps, script);
			case (((uint64_t) 'em' << 32) | (uint64_t) 'ulov'):
				// volume
				return phoneme_script_run_conctrl_volume(ps, script);
			case (((uint64_t) 'en' << 32) | (uint64_t) 'ilfb'):
				// bfline
				return phoneme_script_run_conctrl_bfline(ps, script);
			case (((uint64_t) 'pe' << 32) | (uint64_t) 'tsfb'):
				// bfstep
				return phoneme_script_run_conctrl_bfstep(ps, script);
			case (((uint64_t) 'tr' << 32) | (uint64_t) 'opmi'):
				// import
				return phoneme_script_run_conctrl_import(ps, script);
			case (((uint64_t) 'edu' << 32) | (uint64_t) 'lcni'):
				// include
				return phoneme_script_run_conctrl_include(ps, po, script);
			case (((uint64_t) 'ega' << 32) | (uint64_t) 'kcap'):
				// package
				return phoneme_script_run_conctrl_package(ps, script);
			case (((uint64_t) 'eme' << 32) | (uint64_t) 'nohp'):
				// phoneme
				return phoneme_script_run_conctrl_phoneme(ps, script);
			case (((uint64_t) 'erf' << 32) | (uint64_t) 'pmas'):
				// sampfre
				return phoneme_script_run_conctrl_sampfre(ps, po, script);
		}
	}
	mlog_printf(ps->mlog, "unknow control key [%s]\n", key);
	return NULL;
}

static const char* phoneme_script_run_tpos_modify(phoneme_script_s *restrict ps, const char *restrict script)
{
	double n;
	n = ps->space_time;
	if (*script == '(')
	{
		++script;
		while (phoneme_alpha_table_space[*(uint8_t*)script]) ++script;
		n = phoneme_script_run_read_double(ps, &script);
		if (!script) goto Err;
		while (phoneme_alpha_table_space[*(uint8_t*)script]) ++script;
		if (*script == '=')
		{
			ps->space_time = n;
			++script;
			while (phoneme_alpha_table_space[*(uint8_t*)script]) ++script;
		}
		if (*script != ')')
		{
			Err:
			return NULL;
		}
		++script;
	}
	if ((ps->curr_pos += n * ps->base_time) < 0)
		ps->curr_pos = 0;
	return script;
}

static const char* phoneme_script_run_tpos_ulast(phoneme_script_s *restrict ps, const char *restrict script)
{
	json_inode_t *value;
	ps->last_pos = ps->curr_pos;
	if (*script == '<')
	{
		char *jpath;
		char path[phoneme_var_path_max];
		++script;
		value = NULL;
		jpath = NULL;
		if (!phoneme_read_string(path, sizeof(path), &script, ">")) goto Err;
		if (*script != '>') goto Err;
		++script;
		if (*path) jpath = path;
		value = json_create_floating(ps->curr_pos);
		if (!value) goto Err;
		if (!phoneme_pool_set_var(ps->phoneme_pool, jpath, value)) goto Err;
	}
	return script;
	Err:
	if (value) json_free(value);
	return NULL;
}

static const char* phoneme_script_run_tpos_ucurr(phoneme_script_s *restrict ps, const char *restrict script)
{
	double pos;
	pos = ps->last_pos;
	if (*script == '<')
	{
		pos = phoneme_script_run_read_double(ps, &script);
		if (!script) goto Err;
		if (pos < 0) pos = 0;
	}
	ps->curr_pos = pos;
	return script;
	Err:
	return NULL;
}

static const char* phoneme_script_run_phoneme_note(phoneme_script_s *restrict ps, const char *restrict script, phoneme_output_s *restrict po, note_s *note)
{
	double klength;
	double kvolume;
	double kstep;
	double v;
	klength = kvolume = 1;
	kstep = 0;
	while (*script)
	{
		switch (*script)
		{
			case '_':
				++script;
				klength = phoneme_script_run_read_double(ps, &script);
				if (klength <= 0) goto Err;
				break;
			case '^':
				++script;
				kvolume = phoneme_script_run_read_double(ps, &script);
				if (kvolume <= 0) goto Err;
				break;
			case '~':
				++script;
				if (*script >= 'a' && *script <= 'z')
					v = (*script++ - 'a' + 1) * ps->base_fre_step;
				else if (*script >= 'A' && *script <= 'Z')
					v = - (*script++ - 'A' + 1) * ps->base_fre_step;
				else v = 0;
				kstep = v + phoneme_script_run_read_double(ps, &script);
				break;
			case '=':
				++script;
				ps->base_time *= klength;
				ps->base_volume *= kvolume;
				ps->base_fre_line *= exp2(kstep / ps->base_fre_step);
				goto End;
			case '-':
				++script;
				// fall through
			case ')':
			case ',':
			case ':':
			case ';':
			case ' ':
			case '\t':
			case '\r':
			case '\n':
				if (!phoneme_output_gen_note(po, note, ps->curr_pos, klength *= ps->base_time, kvolume *= ps->base_volume, kstep = ps->base_fre_line * exp2(kstep / ps->base_fre_step)))
				{
					mlog_printf(
						ps->mlog, "phoneme gen note fail, [pos = %.2f, length = %.3f, volume = %.3f, fre = %.2f]\n",
						ps->curr_pos, klength, kvolume, kstep
					);
					goto Err;
				}
				ps->curr_pos += klength;
				End:
				return script;
			default:
				goto Err;
		}
		if (!script) goto Err;
	}
	Err:
	return NULL;
}

static phoneme_s* phoneme_script_modify_phoneme(phoneme_script_s *restrict ps, const char *restrict *restrict pscript, phoneme_s *restrict p)
{
	p = phoneme_modify(p, ps->phoneme_pool, pscript, ps->sdmax);
	if (p)
	{
		if (phoneme_update(p, ps->phoneme_pool, ps->dmax))
			return p;
		else mlog_printf(ps->mlog, "phoneme update fail ...\n");
		refer_free(p);
	}
	else mlog_printf(ps->mlog, "phoneme modify fail ...\n");
	return NULL;
}

static void phoneme_script_run_dump_pos(mlog_s *restrict mlog, const char *restrict start, const char *restrict pos);
static const char* phoneme_script_run_phoneme_do(phoneme_script_s *restrict ps, const char *restrict *restrict pscript, phoneme_output_s *restrict po, phoneme_s *restrict *restrict pcp, phoneme_s *restrict *restrict pnp);
static const char* phoneme_script_run_phoneme_var(phoneme_script_s *restrict ps, const char *restrict *restrict pscript, phoneme_output_s *restrict po, phoneme_s *restrict *restrict pcp, phoneme_s *restrict *restrict pnp)
{
	json_inode_t *v, *nf;
	const char *r, *s;
	r = NULL;
	if (ps->vstack_phoneme_this < ps->vstack_phoneme_max)
	{
		++ps->vstack_phoneme_this;
		v = phoneme_pool_read_value(ps->phoneme_pool, pscript, &nf);
		if (v && v->type == json_inode_string)
		{
			if (!nf) nf = json_copy(v);
			if (nf)
			{
				s = nf->value.string;
				if (phoneme_script_run_phoneme_do(ps, &s, po, pcp, pnp) && !*s)
					r = *pscript;
				else
				{
					mlog_printf(ps->mlog, "phoneme[%u] run var fail ...\n", ps->vstack_phoneme_this);
					phoneme_script_run_dump_pos(ps->mlog, nf->value.string, s);
				}
			}
		}
		if (nf) json_free(nf);
		--ps->vstack_phoneme_this;
	}
	else mlog_printf(ps->mlog, "phoneme run var stack over ...\n");
	return r;
}

static const char* phoneme_script_run_phoneme_do(phoneme_script_s *restrict ps, const char *restrict *restrict pscript, phoneme_output_s *restrict po, phoneme_s *restrict *restrict pcp, phoneme_s *restrict *restrict pnp)
{
	register const char *restrict script;
	phoneme_s *p, *np, *tp;
	p = *pcp;
	np = *pnp;
	np = tp = NULL;
	script = *pscript;
	while (*script)
	{
		while (phoneme_alpha_table_space[*(uint8_t*)script]) ++script;
		*pscript = script;
		switch (*script)
		{
			case ')':
				goto End;
			case ',':
				// time curr pos +=
				script = phoneme_script_run_tpos_modify(ps, script + 1);
				if (!phoneme_output_set_frames(po, ps->curr_pos)) goto Err;
				break;
			case ';':
				// time last pos update
				script = phoneme_script_run_tpos_ulast(ps, script + 1);
				break;
			case ':':
				// time curr pos update
				script = phoneme_script_run_tpos_ucurr(ps, script + 1);
				if (!phoneme_output_set_frames(po, ps->curr_pos)) goto Err;
				break;
			case '[':
				// modify phoneme
				*pscript = ++script;
				tp = phoneme_script_modify_phoneme(ps, pscript, p);
				if (!tp) goto Err;
				if (*(script = *pscript) != ']') goto Err;
				if (*++script == '=')
				{
					// np = NULL
					++script;
					if (np)
					{
						refer_free(np);
						np = NULL;
					}
					refer_free(p);
					p = tp;
				}
				else if (np)
				{
					// keep np
					refer_free(p);
					p = tp;
				}
				else
				{
					// np = p
					np = p;
					p = tp;
				}
				tp = NULL;
				break;
			case '<':
				script = phoneme_script_run_phoneme_var(ps, pscript, po, &p, &np);
				break;
			default:
				// phoneme
				script = phoneme_script_run_phoneme_note(ps, script, po, p->note);
				if (np)
				{
					refer_free(p);
					p = np;
					np = NULL;
				}
				break;
		}
		if (!script) goto Err;
	}
	*pscript = script;
	goto End;
	Err:
	script = NULL;
	End:
	*pcp = p;
	*pnp = np;
	if (tp) refer_free(tp);
	return script;
}

static const char* phoneme_script_run_phoneme(phoneme_script_s *restrict ps, const char *restrict *restrict pscript, phoneme_output_s *restrict po)
{
	register const char *restrict script;
	phoneme_s *p, *np;
	double bt, bv, bfl;
	bt = ps->base_time;
	bv = ps->base_volume;
	bfl = ps->base_fre_line;
	np = NULL;
	script = NULL;
	p = phoneme_script_get_phoneme(ps, pscript);
	if (p)
	{
		if (**pscript == '(')
		{
			++*pscript;
			script = phoneme_script_run_phoneme_do(ps, pscript, po, &p, &np);
			if (script && *script == ')') ++script;
			else script = NULL;
		}
		else
		{
			if (phoneme_output_gen_note(po, p->note, ps->curr_pos, ps->base_time, ps->base_volume, ps->base_fre_line))
				script = *pscript;
			else
			{
				mlog_printf(
					ps->mlog, "phoneme gen note fail, [pos = %.2f, length = %.3f, volume = %.3f, fre = %.2f]\n",
					ps->curr_pos, ps->base_time, ps->base_volume, ps->base_fre_line
				);
			}
			ps->curr_pos += ps->base_time;
		}
	}
	if (p) refer_free(p);
	if (np) refer_free(np);
	ps->base_time = bt;
	ps->base_volume = bv;
	ps->base_fre_line = bfl;
	return script;
}

static const char* phoneme_script_run_var(phoneme_script_s *restrict ps, const char *restrict *restrict pscript, phoneme_output_s *restrict po)
{
	json_inode_t *v, *nf;
	const char *r;
	r = NULL;
	if (ps->vstack_script_this < ps->vstack_script_max)
	{
		++ps->vstack_script_this;
		v = phoneme_pool_read_value(ps->phoneme_pool, pscript, &nf);
		if (v && v->type == json_inode_string)
		{
			if (!nf) nf = json_copy(v);
			if (nf)
			{
				if (phoneme_script_run(ps, nf->value.string, po))
					r = *pscript;
			}
		}
		if (nf) json_free(nf);
		--ps->vstack_script_this;
	}
	else mlog_printf(ps->mlog, "script run var stack over ...\n");
	return r;
}

static void phoneme_script_run_dump_pos(mlog_s *restrict mlog, const char *restrict start, const char *restrict pos)
{
	register const char *s;
	const char *b, *e;
	uint32_t line, col, u, v;
	char buffer[64];
	if (mlog)
	{
		if (*pos == '\n' && pos > start) --pos;
		for (s = pos; *s && *s != '\n'; ++s);
		while (s > pos && (*s == '\n' || *s == '\r')) --s;
		e = s;
		for (s = pos; s > start && *s != '\n'; --s);
		if (*s == '\n' && s < pos) ++s;
		b = s;
		line = 1;
		col = (uint32_t)(uintptr_t)(pos - b) + 1;
		while (s > start)
		{
			if (*s == '\n') ++line;
			--s;
		}
		u = (uint32_t)(uintptr_t)(pos - b);
		v = (uint32_t)(uintptr_t)(e - pos);
		if (u + v >= sizeof(buffer))
		{
			b = pos - u * (sizeof(buffer) - 1) / (u + v);
			e = pos + v * (sizeof(buffer) - 1) / (u + v);
		}
		for (s = b, v = 0; s <= e; ++s)
		{
			buffer[v++] = (*s == '\t')?' ':*s;
		}
		buffer[v] = 0;
		mlog_printf(mlog, "%8u:%-4u %s\n", line, col, buffer);
		mlog_printf(mlog, "%*c\n", u + 15, '^');
	}
}

phoneme_output_s* phoneme_script_run(phoneme_script_s *restrict ps, register const char *restrict script, phoneme_output_s *restrict po)
{
	const char *start, *pos;
	start = pos = script;
	while (*script)
	{
		while (phoneme_alpha_table_space[*(uint8_t*)script]) ++script;
		pos = script;
		switch (*script)
		{
			case 0:
				break;
			case '.':
				// control
				script = phoneme_script_run_conctrl(ps, script + 1, po);
				break;
			case ',':
				// time curr pos +=
				script = phoneme_script_run_tpos_modify(ps, script + 1);
				if (!phoneme_output_set_frames(po, ps->curr_pos)) goto Err;
				break;
			case ';':
				// time last pos update
				script = phoneme_script_run_tpos_ulast(ps, script + 1);
				break;
			case ':':
				// time curr pos update
				script = phoneme_script_run_tpos_ucurr(ps, script + 1);
				if (!phoneme_output_set_frames(po, ps->curr_pos)) goto Err;
				break;
			case '<':
				// run var
				script = phoneme_script_run_var(ps, &pos, po);
				break;
			case '#':
				// note
				if (script[1] == '#' && script[2] == '#')
				{
					if (!(script = strstr(script + 3, "###")))
						goto End;
				}
				else if (!(script = strchr(script + 1, '\n'))) goto End;
				break;
			default:
				// phoneme
				script = phoneme_script_run_phoneme(ps, &pos, po);
				break;
		}
		if (!script) goto Err;
	}
	End:
	return po;
	Err:
	mlog_printf(ps->mlog, "script[%u] run fail\n", ps->vstack_script_this);
	phoneme_script_run_dump_pos(ps->mlog, start, pos);
	return NULL;
}
