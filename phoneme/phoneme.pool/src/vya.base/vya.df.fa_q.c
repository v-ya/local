#define _DEFAULT_SOURCE
#include <phoneme/phoneme.h>
#include <float.h>
#include <math.h>
#include "../vya.common/get_float.inc"

typedef struct df_fa_q_s {
	double kf;
	double ka;
} df_fa_q_s;

typedef struct df_fa_q_aa_s {
	double la;
	double sa;
} df_fa_q_aa_s;

typedef struct df_fa_q_data_s {
	double lf;
	double sf;
	df_fa_q_aa_s aa[];
} df_fa_q_data_s;

static double pi2 = M_PI * 2;

static dyl_used phoneme_details_func(df_fa_q, df_fa_q_s*restrict)
{
	register df_fa_q_data_s *fa_q;
	register note_details_saq_s *restrict saq;
	register df_fa_q_aa_s *restrict aa;
	register uint32_t i;
	register double qf, qa, qq;
	if (!pri) goto End;
	if (!(fa_q = (df_fa_q_data_s *) *data))
	{
		if ((*data = fa_q = (df_fa_q_data_s *) refer_alloz(sizeof(df_fa_q_data_s) + sizeof(df_fa_q_aa_s) * d->max)))
		{
			fa_q->lf = arg->f;
			i = d->used;
			saq = d->saq + i;
			aa = fa_q->aa + i;
			while (i)
			{
				--saq;
				--aa;
				aa->la = saq->sa;
				--i;
			}
		}
		else goto End;
	}
	qf = (fa_q->sf += (arg->t * (arg->f - fa_q->lf) * pri->kf));
	fa_q->lf = arg->f;
	i = d->used;
	saq = d->saq + i;
	aa = fa_q->aa + i;
	while (i)
	{
		--saq;
		--aa;
		qq = arg->f * pi2 * i;
		qq = tan(qq + saq->sq - aa->sa) * log(saq->sa / aa->la);
		qa = 0;
		if (finite(qq))
			qa = (aa->sa += (arg->t * qq * pri->ka));
		aa->la = saq->sa;
		saq->sq -= qf + qa;
		--i;
	}
	End:
	return ;
}
dyl_export(df_fa_q, $details$vya.df.fa_q);

static dyl_used phoneme_arg2pri_func(df_fa_q_arg, df_fa_q_s*)
{
	register df_fa_q_s *r;
	r = NULL;
	r = (df_fa_q_s *) refer_alloc(sizeof(df_fa_q_s));
	if (r)
	{
		r->kf = 1;
		r->ka = 0;
		get_float(&r->kf, arg, ".kf");
		get_float(&r->ka, arg, ".ka");
		r->kf *= pi2;
	}
	return r;
}
dyl_export(df_fa_q_arg, $arg2pri$vya.df.fa_q.arg);

