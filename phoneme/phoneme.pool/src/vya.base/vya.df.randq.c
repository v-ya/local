#define _DEFAULT_SOURCE
#include <phoneme/phoneme.h>
#include <stdlib.h>
#include <math.h>

static dyl_used phoneme_details_func(df_randq, refer_t)
{
	if (d->used)
	{
		uint32_t i;
		i = d->used;
		while (i) d->saq[--i].sq = (M_PI * 2 / RAND_MAX) * rand();
	}
}
dyl_export(df_randq, $details$vya.df.randq);

