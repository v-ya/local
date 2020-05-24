#include "phoneme.h"

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
