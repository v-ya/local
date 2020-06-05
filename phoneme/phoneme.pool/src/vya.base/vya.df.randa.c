#include <phoneme/phoneme.h>
#include <stdlib.h>

static dyl_used phoneme_details_func(df_randa, refer_t)
{
	if (d->used)
	{
		uint32_t i;
		i = d->used;
		while (i) d->saq[--i].sa = (1.0 / RAND_MAX) * rand();
	}
}
dyl_export(df_randa, $details$vya.df.randa);

