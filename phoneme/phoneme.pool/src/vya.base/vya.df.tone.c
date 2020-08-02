#include <phoneme/phoneme.h>
#include <math.h>
#include "../vya.common/polyline.inc"
#include "../vya.common/get_float.inc"

typedef struct df_tone_s {
	double fre_start;
	double fre_length;
	size_t fre_number;
	double tone[];
} df_tone_s;

static dyl_used phoneme_details_func(df_tone, register df_tone_s *restrict)
{
	register double bf, df, ss, l;
	register size_t index, n;
	register uint32_t i;
	double ts, te;
	if ((i = d->used))
	{
		bf = arg->basefre;
		if (pri)
		{
			ss = pri->fre_start;
			l = pri->fre_length;
			ts = pri->tone[0];
			te = pri->tone[(n = pri->fre_number) - 1];
			while (i)
			{
				df = i * bf - ss;
				if (df < 0) df = ts;
				else if ((index = (size_t) (df / l))) df = pri->tone[index];
				else df = te;
				d->saq[--i].sa *= exp(df);
			}
		}
	}
}
dyl_export(df_tone, $details$vya.df.tone);

static df_tone_s* df_tone_alloc(refer_t pl, double length, double fre_start, double fre_length, double fre_granularity)
{
	register df_tone_s *restrict r;
	register size_t i, n;
	register double integral, k, l;
	r = NULL;
	if (length > 0 && fre_length > 0 && fre_granularity > 0 && (n = (size_t) (fre_length / fre_granularity)))
	{
		r = (df_tone_s *) refer_alloz(sizeof(df_tone_s) + sizeof(double) * n);
		if (r)
		{
			r->fre_start = fre_start;
			r->fre_length = l = fre_length / n;
			r->fre_number = n;
			integral = 0;
			k = length / n;
			for (i = 0; i < n; ++i)
			{
				r->tone[i] = integral;
				integral += polyline(k * i, pl) * l;
			}
		}
	}
	return r;
}

static dyl_used phoneme_arg2pri_func(df_tone_arg, df_tone_s*)
{
	df_tone_s *r;
	refer_t pl;
	double fre_start;
	double fre_length;
	double fre_granularity;
	double length;
	r = NULL;
	pl = polyline_arg(json_object_find(arg, "tone"), NULL, NULL);
	if (pl)
	{
		fre_start = 0;
		get_float(&fre_start, arg, ".fre.start");
		fre_length = 20000;
		get_float(&fre_length, arg, ".fre.length");
		fre_granularity = 10;
		get_float(&fre_granularity, arg, ".fre.g");
		length = fre_length;
		get_float(&length, arg, ".length");
		r = df_tone_alloc(pl, length, fre_start, fre_length, fre_granularity);
		refer_free(pl);
	}
	return r;
}
dyl_export(df_tone_arg, $arg2pri$vya.df.tone.arg);

