#include <phoneme/phoneme.h>
#include "../vya.common/random_const.inc"

typedef struct dg_rand_s {
	refer_t rca;
	refer_t rcq;
} dg_rand_s;

static dyl_used phoneme_details_func(dg_rand, register dg_rand_s *restrict)
{
	register double f;
	register uint32_t n;
	n = d->used = d->max;
	if (pri) while (n)
	{
		--n;
		d->saq[n].sa = ((f = random_const(pri->rca)) > 0)?f:0;
		d->saq[n].sq = random_const(pri->rcq);
	}
}
dyl_export(dg_rand, $details$vya.dg.rand);

static void dg_rand_free_func(register dg_rand_s *restrict r)
{
	if (r->rca) refer_free(r->rca);
	if (r->rcq) refer_free(r->rcq);
}

static dyl_used phoneme_arg2pri_func(dg_rand_arg, dg_rand_s*)
{
	static const char rgname[] = "vya.rg.normal";
	register dg_rand_s *restrict r;
	r = refer_alloz(sizeof(dg_rand_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) dg_rand_free_func);
		if ((r->rca = random_const_arg(json_object_find(arg, "a"), rgname, NULL)) &&
			(r->rcq = random_const_arg(json_object_find(arg, "q"), rgname, NULL))) ;
		else
		{
			refer_free(r);
			r = NULL;
		}
	}
	return r;
}
dyl_export(dg_rand_arg, $arg2pri$vya.dg.rand.arg);

