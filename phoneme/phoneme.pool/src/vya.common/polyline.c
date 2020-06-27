#include <phoneme/phoneme.h>
#include "get_string.inc"
#include "vdf_find.inc"
#include "seqence.inc"

typedef struct polyline_s {
	size_t n;
	double *s;
	double *v;
	value_deal_f a_func;
	refer_t a_pri;
	value_deal_f b_func;
	refer_t b_pri;
	double value[];
} polyline_s;

static dyl_used double polyline(register double t, register polyline_s *restrict p)
{
	t = seqence(p->n, p->s, p->v, t, p->a_func, p->a_pri);
	if (p->b_func) return p->b_func(t, p->b_pri);
	else return t;
}
dyl_export(polyline, vya.common.polyline);

static void polyline_free_func(register polyline_s *restrict r)
{
	if (r->a_pri) refer_free(r->a_pri);
	if (r->b_pri) refer_free(r->b_pri);
}

static dyl_used polyline_s* polyline_arg(register json_inode_t *restrict arg, const char *restrict asym, const char *restrict bsym)
{
	register polyline_s *restrict r;
	register json_inode_t *v;
	size_t n;
	if (arg && (v = json_find(arg, ".=")) && v->type == json_inode_array && (n = v->value.array.number))
	{
		r = refer_alloz(sizeof(polyline_s) + sizeof(double) * 2 * n);
		if (r)
		{
			phoneme_arg2pri_f arg2pri;
			refer_set_free(r, (refer_free_f) polyline_free_func);
			seqence_arg(r->n = n, r->s = r->value, r->v = r->value + n, v);
			v = json_object_find(arg, "-");
			get_string(&asym, v, ".@");
			if (asym)
			{
				r->a_func = vdf_find(asym, &arg2pri);
				if (!r->a_func) goto Err;
				if (arg2pri)
				{
					r->a_pri = arg2pri(v);
					if (!r->a_pri) goto Err;
				}
			}
			v = json_object_find(arg, "+");
			get_string(&bsym, v, ".@");
			if (bsym)
			{
				r->b_func = vdf_find(bsym, &arg2pri);
				if (!r->b_func) goto Err;
				if (arg2pri)
				{
					r->b_pri = arg2pri(v);
					if (!r->b_pri) goto Err;
				}
			}
			return r;
			Err:
			refer_free(r);
		}
	}
	return NULL;
}
dyl_export(polyline_arg, vya.common.polyline_arg);
