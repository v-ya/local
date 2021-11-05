#include "cparse.type.h"

typedef struct cparse_inner_tstring_number_trigger_strat_s {
	tparse_tstring_trigger_s trigger;
	tparse_tword_edge_s *we_base;
	tparse_tword_edge_s *we_exp;
} cparse_inner_tstring_number_trigger_strat_s;

tparse_tstring_trigger_s* cparse_inner_tstring_number_trigger_strat_trigger_func(cparse_inner_tstring_number_trigger_strat_s *restrict trigger, tparse_tstring_value_t *restrict value, const char *restrict data, uintptr_t size, uintptr_t *restrict pos)
{
	uintptr_t p;
	p = *pos;
	p = tparse_tword_skip(trigger->we_base, data, size, p);
	if (p > *pos && (data[p - 1] == 'e' || data[p - 1] == 'E'))
		p = tparse_tword_skip(trigger->we_exp, data, size, p);
	if (p > *pos && tparse_tstring_value_append(value, data + *pos, p - *pos))
	{
		tparse_tstring_value_finally(value);
		*pos = p;
		return &trigger->trigger;
	}
	return NULL;
}

static void cparse_inner_tstring_number_trigger_strat_free_func(cparse_inner_tstring_number_trigger_strat_s *restrict r)
{
	if (r->we_base)
		refer_free(r->we_base);
	if (r->we_exp)
		refer_free(r->we_exp);
}

static tparse_tstring_trigger_s* cparse_inner_tstring_number_trigger_strat_alloc(void)
{
	cparse_inner_tstring_number_trigger_strat_s *restrict r;
	r = (cparse_inner_tstring_number_trigger_strat_s *) refer_alloz(sizeof(cparse_inner_tstring_number_trigger_strat_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) cparse_inner_tstring_number_trigger_strat_free_func);
		r->we_base = tparse_tword_edge_alloc();
		r->we_exp = tparse_tword_edge_alloc();
		if (r->we_base && r->we_exp)
		{
			tparse_tword_edge_s *e;
			uintptr_t i;
			// base
			e = r->we_base;
			e->edge['+'] = tparse_tword_edge_head;
			e->edge['-'] = tparse_tword_edge_head;
			e->edge['.'] = tparse_tword_edge_head | tparse_tword_edge_inner | tparse_tword_edge_tail;
			for (i = '0'; i <= '9'; ++i)
				e->edge[i] = tparse_tword_edge_head | tparse_tword_edge_inner | tparse_tword_edge_tail;
			for (i = 'A'; i <= 'Z'; ++i)
				e->edge[i] = tparse_tword_edge_inner | tparse_tword_edge_tail;
			for (i = 'a'; i <= 'z'; ++i)
				e->edge[i] = tparse_tword_edge_inner | tparse_tword_edge_tail;
			// exp
			e = r->we_exp;
			e->edge['+'] = tparse_tword_edge_head;
			e->edge['-'] = tparse_tword_edge_head;
			for (i = '0'; i <= '9'; ++i)
				e->edge[i] = tparse_tword_edge_head | tparse_tword_edge_inner | tparse_tword_edge_tail;
			// trigger
			r->trigger.trigger = (tparse_tstring_trigger_f) cparse_inner_tstring_number_trigger_strat_trigger_func;
			return &r->trigger;
		}
		refer_free(r);
	}
	return NULL;
}

tparse_tstring_s* cparse_inner_alloc_tstring_number(void)
{
	tparse_tstring_s *restrict r;
	r = tparse_tstring_alloc_empty(NULL);
	if (r)
	{
		tparse_tstring_trigger_s *restrict t;
		t = cparse_inner_tstring_number_trigger_strat_alloc();
		if (t)
		{
			if (tparse_tstring_set_trigger(r, tparse_tstring_trigger_head, NULL, t))
			{
				refer_free(t);
				return r;
			}
			refer_free(t);
		}
		refer_free(r);
	}
	return NULL;
}
