#include "phoneme_script.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

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
						r->space_time = 1;
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
			else if (r)
			{
				refer_free(r);
				r = NULL;
			}
		}
		else r = phoneme_pool_get_phoneme(ps->phoneme_pool, phname);
	}
	return r;
}


phoneme_buffer_s* phoneme_script_load(phoneme_script_s *restrict ps, const char *restrict script_path, phoneme_buffer_s *restrict pb)
{
	char *script;
	FILE *fp;
	size_t size;
	script = NULL;
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
		pb = phoneme_script_run(ps, script, pb);
		free(script);
	}
	else pb = NULL;
	return pb;
}

static const char* phoneme_script_run_conctrl_var(phoneme_script_s *restrict ps, const char *restrict script)
{
	json_inode_t *value;
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
			value = NULL;
			if ((script = json_decode(script, &value)) && value)
			{
				jpath = NULL;
				if (*path) jpath = path;
				if (phoneme_pool_set_var(ps->phoneme_pool, jpath, value))
					return script;
				json_free(value);
			}
		}
	}
	Err:
	return NULL;
}

static const char* phoneme_script_run_conctrl_core(phoneme_script_s *restrict ps, const char *restrict script)
{
	json_inode_t *value;
	value = NULL;
	script = json_decode(script, &value);
	if (script)
	{
		if (value->type == json_inode_string)
		{
			if (!phoneme_script_set_core_path(ps, value->value.string))
				script = NULL;
		}
		else if (value->type == json_inode_null)
			phoneme_script_set_core_path(ps, NULL);
		else script = NULL;
		json_free(value);
	}
	return script;
}

static const char* phoneme_script_run_conctrl_package(phoneme_script_s *restrict ps, const char *restrict script)
{
	json_inode_t *value;
	value = NULL;
	script = json_decode(script, &value);
	if (script)
	{
		if (value->type == json_inode_string)
		{
			if (!phoneme_script_set_package_path(ps, value->value.string))
				script = NULL;
		}
		else if (value->type == json_inode_null)
			phoneme_script_set_package_path(ps, NULL);
		else script = NULL;
		json_free(value);
	}
	return script;
}

static const char* phoneme_script_run_conctrl_btime(phoneme_script_s *restrict ps, const char *restrict script)
{
	double n;
	n = strtod(script, (char **) &script);
	if (n <= 0) return NULL;
	ps->base_time = n;
	return script;
}

static const char* phoneme_script_run_conctrl_volume(phoneme_script_s *restrict ps, const char *restrict script)
{
	double n;
	n = strtod(script, (char **) &script);
	if (n < 0) return NULL;
	ps->base_volume = n;
	return script;
}

static const char* phoneme_script_run_conctrl_bfline(phoneme_script_s *restrict ps, const char *restrict script)
{
	double n;
	n = strtod(script, (char **) &script);
	if (n <= 0) return NULL;
	ps->base_fre_line = n;
	return script;
}

static const char* phoneme_script_run_conctrl_bfstep(phoneme_script_s *restrict ps, const char *restrict script)
{
	double n;
	n = strtod(script, (char **) &script);
	if (n <= 0) return NULL;
	ps->base_fre_step = n;
	return script;
}

static const char* phoneme_script_run_conctrl_dmax(phoneme_script_s *restrict ps, const char *restrict script)
{
	uint32_t n;
	n = (uint32_t) strtoul(script, (char **) &script, 10);
	if (!n) return NULL;
	ps->dmax = n;
	return script;
}

static const char* phoneme_script_run_conctrl_sdmax(phoneme_script_s *restrict ps, const char *restrict script)
{
	uint32_t n;
	n = (uint32_t) strtoul(script, (char **) &script, 10);
	ps->sdmax = n;
	return script;
}

static const char* phoneme_script_run_conctrl_import(phoneme_script_s *restrict ps, const char *restrict script)
{
	json_inode_t *value;
	value = NULL;
	script = json_decode(script, &value);
	if (script)
	{
		if (value->type == json_inode_string)
		{
			if (!phoneme_script_load_package(ps, value->value.string))
				script = NULL;
		}
		else script = NULL;
		json_free(value);
	}
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

static const char* phoneme_script_run_conctrl_sampfre(phoneme_buffer_s *restrict pb, const char *restrict script)
{
	uint32_t n;
	n = (uint32_t) strtoul(script, (char **) &script, 10);
	if (!phoneme_buffer_set_sampfre(pb, n)) return NULL;
	return script;
}

static const char* phoneme_script_run_conctrl(phoneme_script_s *restrict ps, const char *restrict script, phoneme_buffer_s *restrict pb)
{
	char key[16];
	*(uint64_t *)key = 0;
	if (phoneme_read_string(key, sizeof(key), &script, " \t\r\n"))
	{
		while (phoneme_alpha_table_space[*(uint8_t*)script]) ++script;
		switch (*(uint64_t *)key)
		{
			case ((uint64_t) 'var'):
				// var
				return phoneme_script_run_conctrl_var(ps, script);
			case ((uint64_t) 'core'):
				// core
				return phoneme_script_run_conctrl_core(ps, script);
			case ((uint64_t) 'dmax'):
				// dmax
				return phoneme_script_run_conctrl_dmax(ps, script);
			case (((uint64_t) 'b' << 32) | (uint64_t) 'time'):
				// btime
				return phoneme_script_run_conctrl_btime(ps, script);
			case (((uint64_t) 's' << 32) | (uint64_t) 'dmax'):
				// sdmax
				return phoneme_script_run_conctrl_sdmax(ps, script);
			case (((uint64_t) 'bf' << 32) | (uint64_t) 'line'):
				// bfline
				return phoneme_script_run_conctrl_bfline(ps, script);
			case (((uint64_t) 'bf' << 32) | (uint64_t) 'step'):
				// bfstep
				return phoneme_script_run_conctrl_bfstep(ps, script);
			case (((uint64_t) 'im' << 32) | (uint64_t) 'port'):
				// import
				return phoneme_script_run_conctrl_import(ps, script);
			case (((uint64_t) 'vo' << 32) | (uint64_t) 'lume'):
				// volume
				return phoneme_script_run_conctrl_volume(ps, script);
			case (((uint64_t) 'pac' << 32) | (uint64_t) 'kage'):
				// package
				return phoneme_script_run_conctrl_package(ps, script);
			case (((uint64_t) 'pho' << 32) | (uint64_t) 'neme'):
				// phoneme
				return phoneme_script_run_conctrl_phoneme(ps, script);
			case (((uint64_t) 'sam' << 32) | (uint64_t) 'pfre'):
				// sampfre
				return phoneme_script_run_conctrl_sampfre(pb, script);
		}
	}
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
		if (*script == '<')
		{
			json_inode_t *value;
			char path[phoneme_var_path_max];
			++script;
			if (!phoneme_read_string(path, sizeof(path), &script, ">")) goto Err;
			if (*script != '>') goto Err;
			++script;
			value = phoneme_pool_get_var(ps->phoneme_pool, path);
			if (!value) goto Err;
			if (value->type == json_inode_integer)
				n = value->value.integer;
			else if (value->type == json_inode_floating)
				n = value->value.floating;
			else goto Err;
		}
		else n = strtod(script, (char **) &script);
		while (phoneme_alpha_table_space[*(uint8_t*)script]) ++script;
		if (*script == '=') ps->space_time = n;
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
		json_inode_t *value;
		char path[phoneme_var_path_max];
		++script;
		if (!phoneme_read_string(path, sizeof(path), &script, ">")) goto Err;
		if (*script != '>') goto Err;
		++script;
		value = phoneme_pool_get_var(ps->phoneme_pool, path);
		if (!value) goto Err;
		if (value->type == json_inode_integer)
			pos = value->value.integer;
		else if (value->type == json_inode_floating)
			pos = value->value.floating;
		else goto Err;
		if (pos < 0) pos = 0;
	}
	ps->last_pos = ps->curr_pos;
	ps->curr_pos = pos;
	return script;
	Err:
	return NULL;
}

static double phoneme_script_run_phoneme_note_read_double(phoneme_script_s *restrict ps, register const char *restrict *restrict script)
{
	double r;
	if (**script == '<')
	{
		json_inode_t *value;
		char path[phoneme_var_path_max];
		++*script;
		if (!phoneme_read_string(path, sizeof(path), script, ">")) goto Err;
		if (**script != '>') goto Err;
		++*script;
		value = phoneme_pool_get_var(ps->phoneme_pool, path);
		if (!value) goto Err;
		if (value->type == json_inode_integer)
			r = value->value.integer;
		else if (value->type == json_inode_floating)
			r = value->value.floating;
		else goto Err;
	}
	else r = strtod(*script, (char **) script);
	return r;
	Err:
	*script = NULL;
	return 0;
}

static const char* phoneme_script_run_phoneme_note(phoneme_script_s *restrict ps, const char *restrict script, phoneme_buffer_s *restrict pb, note_s *note)
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
				klength = phoneme_script_run_phoneme_note_read_double(ps, &script);
				if (klength <= 0) goto Err;
				break;
			case '^':
				++script;
				kvolume = phoneme_script_run_phoneme_note_read_double(ps, &script);
				if (kvolume <= 0) goto Err;
				break;
			case '~':
				++script;
				if (*script >= 'a' && *script <= 'z')
					v = (*script++ - 'a' + 1) * ps->base_fre_step;
				else if (*script >= 'A' && *script <= 'Z')
					v = - (*script++ - 'a' + 1) * ps->base_fre_step;
				else v = 0;
				kstep = v + phoneme_script_run_phoneme_note_read_double(ps, &script);
				break;
			case ')':
			case ',':
			case ':':
			case ';':
			case ' ':
			case '\t':
			case '\r':
			case '\n':
				if (!phoneme_buffer_gen_note(pb, note, ps->curr_pos, ps->base_time * klength, ps->base_volume * kvolume, ps->base_fre_line * exp2(kstep / ps->base_fre_step)))
					goto Err;
				return script;
			default:
				goto Err;
		}
		if (!script) goto Err;
	}
	Err:
	return NULL;
}

static const char* phoneme_script_run_phoneme(phoneme_script_s *restrict ps, const char *restrict script, phoneme_buffer_s *restrict pb)
{
	phoneme_s *p;
	p = phoneme_script_get_phoneme(ps, &script);
	if (p && p->note)
	{
		if (*script == '(')
		{
			++script;
			while (*script)
			{
				while (phoneme_alpha_table_space[*(uint8_t*)script]) ++script;
				switch (*script)
				{
					case ')':
						return script + 1;
					case ',':
						// time curr pos +=
						script = phoneme_script_run_tpos_modify(ps, script + 1);
						if (!phoneme_buffer_set_frames(pb, ps->curr_pos)) goto Err;
						break;
					case ';':
						// time last pos update
						script = phoneme_script_run_tpos_ulast(ps, script + 1);
						break;
					case ':':
						// time curr pos update
						script = phoneme_script_run_tpos_ucurr(ps, script + 1);
						if (!phoneme_buffer_set_frames(pb, ps->curr_pos)) goto Err;
						break;
					default:
						// phoneme
						script = phoneme_script_run_phoneme_note(ps, script, pb, p->note);
						break;
				}
				if (!script) goto Err;
			}
		}
		else
		{
			if (!phoneme_buffer_gen_note(pb, p->note, ps->curr_pos, ps->base_time, ps->base_volume, ps->base_fre_line))
				goto Err;
			return script;
		}
	}
	Err:
	return NULL;
}

phoneme_buffer_s* phoneme_script_run(phoneme_script_s *restrict ps, register const char *restrict script, phoneme_buffer_s *restrict pb)
{
	while (*script)
	{
		while (phoneme_alpha_table_space[*(uint8_t*)script]) ++script;
		switch (*script)
		{
			case '.':
				// control
				script = phoneme_script_run_conctrl(ps, script + 1, pb);
				break;
			case ',':
				// time curr pos +=
				script = phoneme_script_run_tpos_modify(ps, script + 1);
				if (!phoneme_buffer_set_frames(pb, ps->curr_pos)) goto Err;
				break;
			case ';':
				// time last pos update
				script = phoneme_script_run_tpos_ulast(ps, script + 1);
				break;
			case ':':
				// time curr pos update
				script = phoneme_script_run_tpos_ucurr(ps, script + 1);
				if (!phoneme_buffer_set_frames(pb, ps->curr_pos)) goto Err;
				break;
			default:
				// phoneme
				script = phoneme_script_run_phoneme(ps, script, pb);
				break;
		}
		if (!script) goto Err;
	}
	return pb;
	Err:
	return NULL;
}
