#include "cparse.type.h"

static tparse_tmapping_s* cparse_inner_alloc_tmapping_parse_add_key(tparse_tmapping_s *restrict mp)
{
	tparse_tmapping_s *ret;
	cparse_parse_s *restrict p;
	ret = NULL;
	p = cparse_inner_alloc_parse_key();
	if (p)
	{
		uintptr_t i;
		char name[2];
		name[1] = 0;
		for (i = 'A'; i <= 'Z'; ++i)
		{
			name[0] = (char) i;
			if (!mp->add(mp, name, p))
				goto label_fail;
		}
		for (i = 'a'; i <= 'z'; ++i)
		{
			name[0] = (char) i;
			if (!mp->add(mp, name, p))
				goto label_fail;
		}
		if (!mp->add(mp, "_", p))
			goto label_fail;
		if (!mp->add(mp, "$", p))
			goto label_fail;
		ret = mp;
		label_fail:
		refer_free(p);
	}
	return ret;
}

static tparse_tmapping_s* cparse_inner_alloc_tmapping_parse_add_operator(tparse_tmapping_s *restrict mp)
{
	tparse_tmapping_s *ret;
	cparse_parse_s *restrict p;
	ret = NULL;
	p = cparse_inner_alloc_parse_operator();
	if (p)
	{
		#define d_add(_name)  if (!mp->add(mp, _name, p)) goto label_fail
		d_add("!");
		d_add("%");
		d_add("*");
		d_add("=");
		d_add("^");
		d_add("/");
		d_add("&");
		d_add("|");
		d_add("+");
		d_add("-");
		d_add("<");
		d_add(">");
		d_add("(");
		d_add(")");
		d_add(",");
		d_add(".");
		d_add(":");
		d_add(";");
		d_add("?");
		d_add("[");
		d_add("]");
		d_add("~");
		#undef d_add
		ret = mp;
		label_fail:
		refer_free(p);
	}
	return ret;
}

static tparse_tmapping_s* cparse_inner_alloc_tmapping_parse_add(tparse_tmapping_s *restrict mp, const char *restrict name, cparse_parse_s* (*func)(void))
{
	tparse_tmapping_s *ret;
	cparse_parse_s *restrict p;
	ret = NULL;
	p = func();
	if (p)
	{
		if (mp->add(mp, name, p))
			ret = mp;
		refer_free(p);
	}
	return ret;
}

tparse_tmapping_s* cparse_inner_alloc_tmapping_parse(void)
{
	tparse_tmapping_s *restrict r;
	r = tparse_tmapping_alloc_single();
	if (r)
	{
		#define d_func(_n, _f)  cparse_inner_alloc_tmapping_parse_add(r, _n, cparse_inner_alloc_parse_##_f)
		if (
			cparse_inner_alloc_tmapping_parse_add_key(r) &&
			cparse_inner_alloc_tmapping_parse_add_operator(r) &&
			d_func("#", pre) &&
			d_func("\"", string) &&
			d_func("\'", chars)
		) return r;
		#undef d_func
		refer_free(r);
	}
	return NULL;
}
