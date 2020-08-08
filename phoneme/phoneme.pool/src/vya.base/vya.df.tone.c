#include <phoneme/phoneme.h>
#include <math.h>
#include <memory.h>
#include <alloca.h>
#include "../vya.common/seqarray.inc"
#include "../vya.common/get_float.inc"

static dyl_used phoneme_details_func(df_tone, register refer_t)
{
	register double bf, fre;
	register uint32_t i;
	if ((i = d->used))
	{
		bf = arg->basefre;
		if (pri)
		{
			while (i)
			{
				fre = i * bf;
				d->saq[--i].sa *= exp(seqarray(pri, fre));
			}
		}
	}
}
dyl_export(df_tone, $details$vya.df.tone);

static refer_t df_tone_alloc(refer_t sa, double fre_length, double fre_offset, double fre_granularity)
{
	register refer_t r;
	register double *restrict value;
	register size_t i, n;
	register double integral, k;
	r = NULL;
	if (fre_length > 0 && fre_granularity > 0 && (n = (size_t) (fre_length / fre_granularity)))
	{
		value = (double *) alloca(i = sizeof(double) * n);
		if (value)
		{
			memset(value, 0, i);
			integral = 0;
			k = fre_length / n;
			for (i = 0; i < n; ++i)
			{
				value[i] = integral;
				integral += seqarray(sa, k * i) * k;
			}
			r = seqarray_alloc(value, n, -fre_offset, fre_length);
		}
	}
	return r;
}

static dyl_used phoneme_arg2pri_func(df_tone_arg, refer_t)
{
	refer_t r, pl;
	double fre_offset;
	double fre_length;
	double fre_granularity;
	r = NULL;
	fre_length = 20000;
	fre_offset = 0;
	fre_granularity = 10;
	get_float(&fre_length, arg, ".length");
	get_float(&fre_offset, arg, ".offset");
	get_float(&fre_granularity, arg, ".g");
	pl = seqarray_arg(json_object_find(arg, "tone"), 0, fre_length);
	if (pl)
	{
		r = df_tone_alloc(pl, fre_length, fre_offset, fre_granularity);
		refer_free(pl);
	}
	return r;
}
dyl_export(df_tone_arg, $arg2pri$vya.df.tone.arg);

