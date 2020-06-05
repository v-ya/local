#include "phoneme.h"
#include <stdlib.h>
#include <string.h>

void phoneme_src_unset(register phoneme_src_t *restrict ps)
{
	if (ps->name)
	{
		refer_free((refer_t) ps->name);
		ps->name = NULL;
	}
	if (ps->arg)
	{
		refer_free(ps->arg);
		ps->arg = NULL;
	}
	ps->func = NULL;
	if (ps->pri)
	{
		refer_free(ps->pri);
		ps->pri = NULL;
	}
}

phoneme_src_t* phoneme_src_set(register phoneme_src_t *restrict ps, register phoneme_pool_s *restrict pp, hashmap_t *restrict symlist, const char *restrict type, const char *restrict name)
{
	phoneme_src_name_s sname;
	phoneme_arg_s *restrict arg;
	void *func;
	refer_t pri;
	phoneme_src_unset(ps);
	sname = NULL;
	arg = NULL;
	pri = NULL;
	func = hashmap_get_name(symlist, type);
	if (!func) goto Err;
	sname = phoneme_pool_get_name(pp, type);
	if (!sname) goto Err;
	if (name)
	{
		arg = phoneme_pool_get_arg(pp, type, name);
		pri = phoneme_pool_get_pri(pp, type, name);
		if (!arg || !pri) goto Err;
	}
	ps->name = sname;
	ps->arg = arg;
	ps->func = func;
	ps->pri = pri;
	return ps;
	Err:
	if (sname) refer_free((refer_t) sname);
	if (arg) refer_free(arg);
	if (pri) refer_free(pri);
	return NULL;
}

static void phoneme_free_func(register phoneme_s *restrict p)
{
	register phoneme_src_t *restrict d;
	register uint32_t n;
	if (p->note) refer_free(p->note);
	phoneme_src_unset(&p->envelope);
	phoneme_src_unset(&p->basefre);
	d = p->details;
	n = p->details_max;
	while (n)
	{
		phoneme_src_unset(d);
		--n;
		++d;
	}
}

static phoneme_s* phoneme_set_arg_envelope(register phoneme_s *restrict p, register phoneme_pool_s *restrict pp, json_inode_t *restrict o)
{
	const char *s_sym, *s_arg;
	if (json_get_string(o, "^.sym", &s_sym))
	{
		s_arg = NULL;
		json_get_string(o, "^.arg", &s_arg);
		if (!phoneme_src_set(&p->envelope, pp, &pp->envelope, s_sym, s_arg))
			return NULL;
	}
	return p;
}

static phoneme_s* phoneme_set_arg_basefre(register phoneme_s *restrict p, register phoneme_pool_s *restrict pp, json_inode_t *restrict o)
{
	const char *s_sym, *s_arg;
	if (json_get_string(o, "~.sym", &s_sym))
	{
		s_arg = NULL;
		json_get_string(o, "~.arg", &s_arg);
		if (!phoneme_src_set(&p->basefre, pp, &pp->basefre, s_sym, s_arg))
			return NULL;
	}
	return p;
}

static phoneme_s* phoneme_set_arg_details(register phoneme_s *restrict p, register phoneme_pool_s *restrict pp, json_inode_t *restrict o)
{
	json_inode_t *restrict d;
	const char *s_sym, *s_arg;
	uint32_t i, n;
	d = json_object_find(o, "@");
	if (d && d->type == json_inode_array)
	{
		n = d->value.array.number;
		if (n > p->details_max) goto Err;
		i = p->details_used;
		while (i > n)
		{
			--i;
			phoneme_src_unset(p->details + i);
		}
		p->details_used = n;
		for (i = 0; i < n; ++i)
		{
			o = json_array_find(d, i);
			if (!o) goto Err;
			if (!json_get_string(o, ".sym", &s_sym)) goto Err;
			s_arg = NULL;
			json_get_string(o, ".arg", &s_arg);
			if (!phoneme_src_set(p->details + i, pp, &pp->details, s_sym, s_arg))
				goto Err;
		}
	}
	return p;
	Err:
	return NULL;
}

phoneme_s* phoneme_alloc(uint32_t sdmax, phoneme_pool_s *restrict pp, json_inode_t *restrict o)
{
	phoneme_s *restrict r;
	r = NULL;
	if (pp && o)
	{
		json_inode_t *restrict d;
		d = json_object_find(o, "@");
		if (d && d->type != json_inode_array) d = NULL;
		if (!sdmax && d) sdmax = d->value.array.number;
	}
	else o = NULL;
	if (sdmax)
	{
		r = refer_alloz(sizeof(phoneme_s) + sizeof(phoneme_src_t) * sdmax);
		if (r)
		{
			r->details_max = sdmax;
			refer_set_free(r, (refer_free_f) phoneme_free_func);
			if (!o || phoneme_set_arg(r, pp, o))
				return r;
			refer_free(r);
		}
	}
	return NULL;
}

phoneme_s* phoneme_set_arg(register phoneme_s *restrict p, register phoneme_pool_s *restrict pp, json_inode_t *restrict o)
{
	if (phoneme_set_arg_envelope(p, pp, o) &&
		phoneme_set_arg_basefre(p, pp, o) &&
		phoneme_set_arg_details(p, pp, o))
		return p;
	return NULL;
}

note_s* phoneme_update(register phoneme_s *restrict p, register phoneme_pool_s *restrict pp, uint32_t dmax)
{
	register note_s *restrict n;
	uint32_t i, nn;
	if (!(n = p->note) || n->stage_max < p->details_max || !n->details)
	{
		if (n) refer_free(n);
		p->note = n = note_alloc(dmax, p->details_max);
	}
	else if (n->details->max != dmax)
		n = note_set_details_max(n, dmax);
	if (n)
	{
		note_set_envelope(n, (note_envelope_f) p->envelope.func, p->envelope.pri);
		note_set_base_frequency(n, (note_base_frequency_f) p->basefre.func, p->basefre.pri);
		note_clear_stage(n);
		for (i = 0, nn = p->details_used; i < nn; ++i)
		{
			if (p->details[i].func)
				note_append_stage(n, p->details[i].func, p->details[i].pri);
		}
	}
	return n;
}

static phoneme_src_t* phoneme_modify_script(phoneme_src_t *restrict r, phoneme_src_t *restrict s, phoneme_pool_s *restrict pp, hashmap_t *restrict symlist, register const char *restrict *restrict modify)
{
	register const char *restrict ss, *restrict tt;
	void *func;
	phoneme_src_name_s name;
	json_inode_t *v;
	char path[phoneme_var_path_max];
	char value[phoneme_var_path_max];
	ss = *modify;
	while (phoneme_alpha_table_space[*(uint8_t*)ss]) ++ss;
	if (*ss != '(') goto Err;
	*modify = ss + 1;
	while (*(ss = *modify))
	{
		while (phoneme_alpha_table_space[*(uint8_t*)ss]) ++ss;
		switch (*ss)
		{
			case '=':
				++ss;
				while (phoneme_alpha_table_space[*(uint8_t*)ss]) ++ss;
				if (*ss == '<')
				{
					*modify = ss + 1;
					if (!phoneme_read_string(value, sizeof(value), modify, ">,)"))
						goto Err;
					v = phoneme_pool_get_var(pp, value);
					if (*(ss = *modify) != '>') goto Err;
					if (!v || v->type != json_inode_string) goto Err;
					tt = v->value.string;
					++ss;
				}
				else
				{
					*modify = ss;
					if (!phoneme_read_string(value, sizeof(value), modify, ", \t\r\n)"))
						goto Err;
					tt = value;
					ss = *modify;
				}
				func = hashmap_get_name(symlist, tt);
				if (!func) goto Err;
				name = phoneme_pool_get_name(pp, tt);
				if (!name) goto Err;
				if (r->name) refer_free((refer_t) r->name);
				r->name = name;
				r->func = func;
				break;
			case '<':
				*modify = ss + 1;
				if (!phoneme_read_string(path, sizeof(path), modify, ">,)"))
					goto Err;
				if (*(ss = *modify) != '>') goto Err;
				++ss;
				while (phoneme_alpha_table_space[*(uint8_t*)ss]) ++ss;
				if (*(*modify = ss) != '=') goto Err;
				++ss;
				while (phoneme_alpha_table_space[*(uint8_t*)ss]) ++ss;
				if (*ss == '<')
				{
					*modify = ss + 1;
					if (!phoneme_read_string(value, sizeof(value), modify, ">,)"))
						goto Err;
					if (*(ss = *modify) != '>') goto Err;
					++ss;
					v = phoneme_pool_get_var(pp, value);
					if (!v) goto Err;
					v = json_copy(v);
					if (!v) goto Err;
				}
				else
				{
					v = NULL;
					ss = json_decode(*modify = ss, &v);
					if (!ss || !v) goto Err;
				}
				if (*path)
				{
					if (!r->arg)
					{
						if (s && s->arg) r->arg = phoneme_arg_dump(s->arg);
						else r->arg = phoneme_arg_alloc(NULL);
						if (!r->arg) goto Err_free_v;
					}
					if (!json_set(r->arg, path, v)) goto Err_free_v;
				}
				else
				{
					if (r->arg)
					{
						if (*r->arg) json_free(*r->arg);
						*r->arg = v;
					}
					else if (!(r->arg = phoneme_arg_alloc(v)))
					{
						Err_free_v:
						json_free(v);
						goto Err;
					}
				}
				*modify = ss;
				break;
			default:
				*modify = ss;
				goto Err;
		}
		while (phoneme_alpha_table_space[*(uint8_t*)ss]) ++ss;
		if (*ss == ')')
		{
			*modify = ss + 1;
			return r;
		}
		if (*ss != ',') goto Err;
		*modify = ss + 1;
	}
	Err:
	return NULL;
}

static phoneme_src_t* phoneme_modify_link(register phoneme_src_t *restrict r, register phoneme_src_t *restrict s, register phoneme_pool_s *restrict pp)
{
	if (!r->name)
	{
		if (!(r->name = refer_save((refer_t) s->name))) goto Err;
		r->func = s->func;
	}
	if (!r->arg)
	{
		r->arg = refer_save(s->arg);
		if (r->pri)
		{
			refer_free(r->pri);
			r->pri = NULL;
		}
		if (!r->arg || (s->name && !strcmp(r->name, s->name)))
			r->pri = refer_save(s->pri);
		else goto label_arg2pri;
	}
	else if (!r->pri)
	{
		register phoneme_arg2pri_f arg2pri;
		label_arg2pri:
		arg2pri = phoneme_pool_get_arg2pri(pp, r->name);
		if (!arg2pri) goto Err;
		r->pri = arg2pri(*r->arg);
	}
	return r;
	Err:
	return NULL;
}

phoneme_s* phoneme_modify(register phoneme_s *restrict p, register phoneme_pool_s *restrict pp, const char *restrict *restrict modify, uint32_t sdmax)
{
	static phoneme_src_t s_phoneme_src_null = {
		.name = NULL,
		.arg = NULL,
		.func = NULL,
		.pri = NULL
	};
	register phoneme_s *restrict r;
	register const char *restrict s;
	uint32_t i;
	if (!sdmax) sdmax = p->details_max;
	r = phoneme_alloc(sdmax, NULL, NULL);
	if (!r) return NULL;
	while (*(s = *modify))
	{
		while (phoneme_alpha_table_space[*(uint8_t*)s]) ++s;
		switch (*s)
		{
			case '^':
				++s;
				*modify = s;
				if (!phoneme_modify_script(&r->envelope, &p->envelope, pp, &pp->envelope, modify))
					goto Err_envelope;
				continue;
			case '~':
				++s;
				*modify = s;
				if (!phoneme_modify_script(&r->basefre, &p->basefre, pp, &pp->basefre, modify))
					goto Err_basefre;
				continue;
			case '@':
				++s;
				while (phoneme_alpha_table_space[*(uint8_t*)s]) ++s;
				*modify = s;
				i = strtoul(s, (char **) modify, 10);
				if (!*modify)
				{
					*modify = s;
					goto Err_details;
				}
				if (i >= r->details_max) goto Err;
				if (i >= r->details_used) r->details_used = i + 1;
				if (!phoneme_modify_script(r->details + i, (i < p->details_used)?(p->details + i):NULL, pp, &pp->details, modify))
					goto Err_details;
				continue;
		}
		break;
	}
	if (!phoneme_modify_link(&r->envelope, &p->envelope, pp)) goto Err_link_envelope;
	if (!phoneme_modify_link(&r->basefre, &p->basefre, pp)) goto Err_link_basefre;
	sdmax = p->details_used;
	if (sdmax > r->details_max) sdmax = r->details_max;
	if (r->details_used < sdmax) r->details_used = sdmax;
	for (i = 0; i < sdmax; ++i)
	{
		if (!phoneme_modify_link(r->details + i, p->details + i, pp))
			goto Err_link_details;
	}
	while (i < r->details_used)
	{
		if (!phoneme_modify_link(r->details + i, &s_phoneme_src_null, pp))
			goto Err_link_details;
		++i;
	}
	return r;
	Err:
	refer_free(r);
	return NULL;
	Err_envelope:
	mlog_printf(pp->mlog, "phoneme modify envelope fail ...\n");
	goto Err;
	Err_basefre:
	mlog_printf(pp->mlog, "phoneme modify basefre fail ...\n");
	goto Err;
	Err_details:
	mlog_printf(pp->mlog, "phoneme modify details fail ...\n");
	goto Err;
	Err_link_envelope:
	mlog_printf(pp->mlog, "phoneme modify link envelope fail ...\n");
	goto Err;
	Err_link_basefre:
	mlog_printf(pp->mlog, "phoneme modify link basefre fail ...\n");
	goto Err;
	Err_link_details:
	mlog_printf(pp->mlog, "phoneme modify link details fail ...\n");
	goto Err;
}
